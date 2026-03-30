#pragma once

#include "IDistanceStrategy.hpp"
#include "IMovementStrategy.hpp"
#include "Position.hpp"
#include "Unit.hpp"

#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/EventLog.hpp>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace sw::core
{
	class GameMap
	{
	public:
		GameMap(
			uint32_t width,
			uint32_t height,
			EventLog& eventLog,
			std::unique_ptr<IDistanceStrategy> distanceStrategy,
			std::unique_ptr<IMovementStrategy> movementStrategy) :
				_width(width),
				_height(height),
				_eventLog(eventLog),
				_distanceStrategy(std::move(distanceStrategy)),
				_movementStrategy(std::move(movementStrategy))
		{}

		uint32_t width() const noexcept
		{
			return _width;
		}

		uint32_t height() const noexcept
		{
			return _height;
		}

		void setCurrentTick(uint32_t tick) noexcept
		{
			_currentTick = tick;
		}

		double distance(const Position& from, const Position& to)
		{
			if (!_distanceStrategy)
			{
				throw std::runtime_error("Error: No distance strategy");
			}
			return _distanceStrategy->calculate(from, to);
		}

		void addUnit(std::shared_ptr<Unit> unit)
		{
			unit->setCreationOrder(_nextCreationOrder++);
			_units.push_back(std::move(unit));
		}

		Unit* findUnit(uint32_t id)
		{
			for (auto& unit : _units)
			{
				if (unit->id() == id)
				{
					return unit.get();
				}
			}
			return nullptr;
		}

		const std::vector<std::shared_ptr<Unit>>& units() const noexcept
		{
			return _units;
		}

		bool isOccupied(const Position& pos, const Unit* exclude = nullptr) const
		{
			for (const auto& unit : _units)
			{
				if (unit.get() == exclude)
				{
					continue;
				}
				if (unit->position() == pos && unit->occupiesCell())
				{
					return true;
				}
			}
			return false;
		}

		// Find units within a distance range from 'from', excluding 'self'
		std::vector<Unit*> findUnitsInRange(
			const Position& from, double minDist, double maxDist, const Unit* self = nullptr) const
		{
			std::vector<Unit*> result;
			for (const auto& unit : _units)
			{
				if (unit.get() == self)
				{
					continue;
				}
				double dist = _distanceStrategy->calculate(from, unit->position());
				if (dist >= minDist && dist <= maxDist)
				{
					result.push_back(unit.get());
				}
			}
			return result;
		}

		// Find units in adjacent cells (distance <= sqrt(2), i.e. 8 max neighbors)
		std::vector<Unit*> findAdjacentUnits(const Position& from, const Unit* self = nullptr) const
		{
			return findUnitsInRange(from, 0.0, 1.0, self);
		}

		// Remove units with HP <= 0
		void removeDeadUnits()
		{
			for (const auto& unit : _units)
			{
				if (!unit->isAlive())
				{
					_eventLog.log(_currentTick, io::UnitDied{unit->id()});
				}
			}
			_units.erase(
				std::remove_if(
					_units.begin(),
					_units.end(),
					[](const auto& unit) { return /*unit->hasHp() &&*/ !unit->isAlive(); }),
				_units.end());
		}

		// Move a unit towards its march target by stepsPerTurn cells.
		// Returns true if movement happened.
		bool moveUnitTowardsTarget(Unit& unit)
		{
			Position nextPos = _movementStrategy->nextPos(unit.position(), unit.marchTarget(), unit.stepsPerTurn());

			// Check if target cell is occupied (for units that occupy cells)
			if (isOccupied(nextPos, &unit))
			{
				// TODO: Try to find an alternative adjacent cell closer to target
				// Now, for simplicity, stay in place if blocked (subject to "deadlocks"? or, most likely, premature shutdown)
				// Currently, there will be some interaction with the "occupant" instead on movement, but this is not an overall guarantee
				return false;
			}

			unit.setPosition(nextPos);
			_eventLog.log(_currentTick, io::UnitMoved{unit.id(), nextPos.x, nextPos.y});

			if (nextPos == unit.marchTarget())
			{
				_eventLog.log(_currentTick, io::MarchEnded{unit.id(), nextPos.x, nextPos.y});
				unit.clearMarchTarget();
			}

			return true;
		}

		// Log an attack event
		void logAttack(uint32_t attackerId, uint32_t targetId, uint32_t damage, uint32_t targetHpAfter)
		{
			_eventLog.log(_currentTick, io::UnitAttacked{attackerId, targetId, damage, targetHpAfter});
		}

		// Pick a random unit from a list (deterministic: pick first one for reproducibility)
		Unit* pickRandomTarget(const std::vector<Unit*>& candidates)
		{
			if (candidates.empty())
			{
				return nullptr;
			}
			// For deterministic behavior matching the example, pick the first candidate
			return candidates.front();
		}

	private:
		uint32_t _width;
		uint32_t _height;
		EventLog& _eventLog;
		std::unique_ptr<IDistanceStrategy> _distanceStrategy;
		std::unique_ptr<IMovementStrategy> _movementStrategy;
		std::vector<std::shared_ptr<Unit>> _units;
		uint32_t _currentTick{0};
		uint32_t _nextCreationOrder{0};
	};
}
