#pragma once

#include <Core/IMovementStrategy.hpp>
#include <Core/Position.hpp>
#include <cmath>

namespace sw::features
{
	// Default movement: move one (or stepsPerTurn) cell(s) in the 8-directional grid toward target
	class DefaultMovement : public core::IMovementStrategy
	{
	public:
		core::Position nextPos(
			const core::Position& current, const core::Position& target, uint32_t stepsPerTurn) const override
		{
			auto pos = current;
			for (uint32_t step = 0; step < stepsPerTurn; ++step)
			{
				if (pos == target)
				{
					break;
				}

				int dx = static_cast<int>(target.x) - static_cast<int>(pos.x);
				int dy = static_cast<int>(target.y) - static_cast<int>(pos.y);

				int sx = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
				int sy = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

				pos.x = static_cast<uint32_t>(static_cast<int>(pos.x) + sx);
				pos.y = static_cast<uint32_t>(static_cast<int>(pos.y) + sy);
			}
			return pos;
		}
	};
}
