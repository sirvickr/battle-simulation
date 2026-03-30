#pragma once

#include <Features/CommandProcessor.hpp>
#include <istream>
#include <memory>

namespace sw
{
	namespace io
	{
		class CommandParser;
	}

	namespace core
	{
		class GameMap;
	}

	namespace features
	{
		using GameMapPtr = std::unique_ptr<core::GameMap>;

		class Game
		{
		public:
			void run(std::istream& stream);

		private:
			std::vector<features::CommandVariant> parseCommands(std::istream& stream);
			GameMapPtr processCommands(const std::vector<features::CommandVariant>& commands, uint32_t setupTick = 1);

		private:
			CommandProcessor cmdProcessor;
		};

	}
}
