#pragma once

#include <Core/IDistanceStrategy.hpp>
#include <Core/Position.hpp>
#include <cmath>

namespace sw::features
{
	// Euclidean metric strategy
	class EuclideanDistance : public core::IDistanceStrategy
	{
	public:
		double calculate(const core::Position& from, const core::Position& to) const override
		{
			double dx = static_cast<double>(from.x) - static_cast<double>(to.x);
			double dy = static_cast<double>(from.y) - static_cast<double>(to.y);
			return std::sqrt(dx * dx + dy * dy);
		}
	};
}
