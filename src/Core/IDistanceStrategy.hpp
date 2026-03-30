#pragma once

#include "Position.hpp"

namespace sw::core
{
	// Strategy interface for distance calculation (parameterizable algorithm template)
	class IDistanceStrategy
	{
	public:
		virtual ~IDistanceStrategy() = default;
		virtual double calculate(const Position& from, const Position& to) const = 0;
	};
}
