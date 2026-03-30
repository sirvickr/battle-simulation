#pragma once

#include "Position.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

namespace sw::core
{
	class IUnitVisitor;
	class GameMap;

	// Base class for all units. Features extend this by adding characteristics and behaviors.
	class Unit
	{
	public:
		Unit(uint32_t id, Position pos, int32_t hp, uint32_t stepsPerTurn) :
				_id(id),
				_position(pos),
				_stepsPerTurn(stepsPerTurn),
				_hp(hp),
				_maxHp(hp)
		{}

		virtual ~Unit() = default;

		uint32_t id() const
		{
			return _id;
		}

		Position position() const
		{
			return _position;
		}

		void setPosition(Position pos)
		{
			_position = pos;
		}

		// March target
		bool hasTarget() const
		{
			return _marchTarget.has_value();
		}

		Position marchTarget() const
		{
			return _marchTarget.value();
		}

		void setMarchTarget(Position target)
		{
			_marchTarget = target;
		}

		void clearMarchTarget()
		{
			_marchTarget.reset();
		}

		int32_t hp() const noexcept
		{
			return _hp;
		}

		void changeHP(int32_t delta)
		{
			_hp += delta;
			if (_hp < 0)
			{
				_hp = 0;
			}
		}

		// virtual bool acceptsHpChange(int32_t amount, const Unit& unit, double distance) const = 0;

		bool isAlive() const noexcept
		{
			return _hp > 0;
		}

		// Whether this unit occupies a cell (blocks others from entering)
		virtual bool occupiesCell() const
		{
			return true;
		}

		// virtual bool canMove() const = 0;

		// How many cells per turn this unit moves
		virtual uint32_t stepsPerTurn() const
		{
			return _stepsPerTurn;
		}

		// Whether this unit can be attacked in melee
		virtual bool canBeAttackedMelee() const
		{
			return true;
		}

		// Whether this unit can be attacked at range
		virtual bool canBeAttackedRange() const
		{
			return true;
		}

		// Whether this unit can be attacked at all
		virtual bool canBeAttacked() const
		{
			return true;
		}

		// Modifier for ranged attack distance when targeting this unit
		virtual int32_t rangeDistanceModifier() const
		{
			return 0;
		}

		// Perform the unit's action for this turn. Returns true if the unit did something meaningful.
		virtual bool act(GameMap& map) = 0;

		// Order in which this unit was created (for turn ordering)
		uint32_t creationOrder() const
		{
			return _creationOrder;
		}

		void setCreationOrder(uint32_t order)
		{
			_creationOrder = order;
		}

	private:
		uint32_t _id{};
		Position _position{};
		uint32_t _stepsPerTurn{};
		std::optional<Position> _marchTarget{};
		uint32_t _creationOrder{};
		int32_t _hp{};
		const int32_t _maxHp;
	};
}
