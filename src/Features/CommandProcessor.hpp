#pragma once

#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <functional>
#include <sstream>
#include <string>
#include <variant>

namespace sw::features
{
	using CommandVariant = std::variant<sw::io::CreateMap, sw::io::SpawnSwordsman, sw::io::SpawnHunter, sw::io::March>;

	class CommandProcessor
	{
	public:
		template <class TCommand>
		CommandProcessor& add(std::function<void(const CommandVariant&)> handler)
		{
			std::string commandName = TCommand::Name;
			auto [it, inserted] = _commands.emplace(commandName, std::move(handler));
			if (!inserted)
			{
				throw std::runtime_error("Command already exists: " + commandName);
			}

			return *this;
		}

		void process(const std::vector<CommandVariant>& commands);

	private:
		std::unordered_map<std::string, std::function<void(const CommandVariant&)>> _commands;
	};

}
