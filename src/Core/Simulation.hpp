#pragma once

#include "GameMap.hpp"

#include <algorithm>
#include <vector>

namespace sw::core
{
	class Simulation
	{
	public:
		explicit Simulation(GameMap& map) :
				_map(map)
		{}

		// Run the simulation until completion
		void run()
		{
			while (true)
			{
				_map.setCurrentTick(++_tick);

				// Snapshot unit list at start of tick (ordered by creation order)
				auto units = _map.units();
				std::sort(
					units.begin(),
					units.end(),
					[](const auto& a, const auto& b) { return a->creationOrder() < b->creationOrder(); });

				bool anyAction = false;

				for (auto& unit : units)
				{
					// Skip if unit was killed or removed during this tick
					if (!_map.findUnit(unit->id()))
					{
						continue;
					}
					// Skip dead units (hp <= 0 from earlier action this tick)
					if (!unit->isAlive())
					{
						continue;
					}

					if (unit->act(_map))
					{
						anyAction = true;
					}

					// After each unit acts, check for newly dead units and log/remove them
					_map.removeDeadUnits();
				}

				// Check termination: only one unit (or none) left
				if (_map.units().size() <= 1)
				{
					break;
				}

				// If nothing happened this tick, simulation is over
				if (!anyAction)
				{
					break;
				}
			}
		}

	private:
		GameMap& _map;
		uint32_t _tick{1};
	};
}
