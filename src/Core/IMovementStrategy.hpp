#pragma once

#include "Position.hpp"

namespace sw::core
{
	// Strategy interface for computing the next cell during movement
	class IMovementStrategy
	{
	public:
		virtual ~IMovementStrategy() = default;
		// Compute the next position when moving from 'current' toward 'target'.
		// 'stepsPerTurn' indicates how many cells the unit can move per turn.
		virtual Position nextPos(const Position& current, const Position& target, uint32_t stepsPerTurn = 1) const = 0;
	};
}
