#include "CommandProcessor.hpp"

namespace sw::features
{
	void CommandProcessor::process(const std::vector<CommandVariant>& commands)
	{
		for (auto& cmd : commands)
		{
			std::visit(
				[&](const auto& command)
				{
					using TCommand = std::decay_t<decltype(command)>;
					std::string commandName = TCommand::Name;
					auto it = _commands.find(commandName);
					if (it == _commands.end())
					{
						throw std::runtime_error("Unknown command: " + commandName);
					}

					it->second(cmd);
				},
				cmd);
		}
	}
}
