#include "Game.hpp"

#include <Core/GameMap.hpp>
#include <Core/Simulation.hpp>
#include <Features/CommandProcessor.hpp>
#include <Features/DefaultMovement.hpp>
#include <Features/EuclideanDistance.hpp>
#include <Features/Hunter.hpp>
#include <Features/Swordsman.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <IO/System/PrintDebug.hpp>
#include <iostream>

namespace sw::features
{
	void Game::run(std::istream& stream)
	{
		std::cout << "Commands:\n";

		auto commands = parseCommands(stream);

		std::cout << "\n\nEvents:\n";

		auto map = processCommands(commands);

		if (map)
		{
			core::Simulation sim(*map);
			sim.run();
		}
	}

	std::vector<features::CommandVariant> Game::parseCommands(std::istream& stream)
	{
		io::CommandParser parser;
		std::vector<features::CommandVariant> commands;
		parser
			.add<io::CreateMap>(
				[&](auto command)
				{
					printDebug(std::cout, command);
					commands.emplace_back(std::move(command));
				})
			.add<io::SpawnSwordsman>(
				[&](auto command)
				{
					printDebug(std::cout, command);
					commands.emplace_back(std::move(command));
				})
			.add<io::SpawnHunter>(
				[&](auto command)
				{
					printDebug(std::cout, command);
					commands.emplace_back(std::move(command));
				})
			.add<io::March>(
				[&](auto command)
				{
					printDebug(std::cout, command);
					commands.emplace_back(std::move(command));
				});

		parser.parse(stream);

		return commands;
	}

	GameMapPtr Game::processCommands(const std::vector<features::CommandVariant>& commands, uint32_t setupTick)
	{
		EventLog eventLog;
		GameMapPtr map;

		cmdProcessor
			.add<io::CreateMap>(
				[&](const auto& commandVar)
				{
					auto& command = std::get<io::CreateMap>(commandVar);
					map = std::make_unique<core::GameMap>(
						command.width,
						command.height,
						eventLog,
						std::make_unique<EuclideanDistance>(),
						std::make_unique<DefaultMovement>());
					map->setCurrentTick(setupTick);
					eventLog.log(setupTick, io::MapCreated{command.width, command.height});
				})
			.add<io::SpawnSwordsman>(
				[&](const auto& commandVar)
				{
					auto& command = std::get<io::SpawnSwordsman>(commandVar);
					if (map)
					{
						auto unit = std::make_shared<Swordsman>(
							command.unitId,
							core::Position{command.x, command.y},
							static_cast<int32_t>(command.hp),
							command.strength);
						map->addUnit(unit);
						eventLog.log(setupTick, io::UnitSpawned{command.unitId, "Swordsman", command.x, command.y});
					}
				})
			.add<io::SpawnHunter>(
				[&](const auto& commandVar)
				{
					auto& command = std::get<io::SpawnHunter>(commandVar);
					if (map)
					{
						auto unit = std::make_shared<Hunter>(
							command.unitId,
							core::Position{command.x, command.y},
							static_cast<int32_t>(command.hp),
							command.agility,
							command.strength,
							command.range);
						map->addUnit(unit);
						eventLog.log(setupTick, io::UnitSpawned{command.unitId, "Hunter", command.x, command.y});
					}
				})
			.add<io::March>(
				[&](const auto& commandVar)
				{
					auto& command = std::get<io::March>(commandVar);
					if (map)
					{
						auto* unit = map->findUnit(command.unitId);
						if (unit)
						{
							eventLog.log(
								setupTick,
								io::MarchStarted{
									command.unitId,
									unit->position().x,
									unit->position().y,
									command.targetX,
									command.targetY});
							unit->setMarchTarget(core::Position{command.targetX, command.targetY});
						}
					}
				});

		cmdProcessor.process(commands);

		return map;
	}
}
