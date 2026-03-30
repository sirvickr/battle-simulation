#pragma once

#include <Core/GameMap.hpp>
#include <Core/Unit.hpp>

namespace sw::features
{
	class Hunter : public core::Unit
	{
	public:
		Hunter(uint32_t id, core::Position pos, int32_t hp, uint32_t agility, uint32_t strength, uint32_t range) :
				core::Unit(id, pos, hp, 1),
				_agility(agility),
				_strength(strength),
				_range(range)
		{}

		bool act(core::GameMap& map) override
		{
			auto adjacent = map.findAdjacentUnits(position(), this);

			bool hasAdjacentUnits = !adjacent.empty();

			if (!hasAdjacentUnits)
			{
				// Ranged attack
				auto rangedTargets = map.findUnitsInRange(position(), 2.0, static_cast<double>(_range), this);
				std::vector<core::Unit*> validTargets;
				for (auto* u : rangedTargets)
				{
					if (!u->canBeAttacked() || !u->canBeAttackedRange())
					{
						continue;
					}
					double dist = map.distance(position(), u->position());
					double effectiveMin = 2.0 + u->rangeDistanceModifier();
					double effectiveMax = static_cast<double>(_range) + u->rangeDistanceModifier();
					if (effectiveMin < 0)
					{
						effectiveMin = 0;
					}
					if (dist >= effectiveMin && dist <= effectiveMax)
					{
						validTargets.push_back(u);
					}
				}

				if (!validTargets.empty())
				{
					auto* target = map.pickRandomTarget(validTargets);
					target->changeHP(-_agility);
					uint32_t hpAfter = target->hp() > 0 ? static_cast<uint32_t>(target->hp()) : 0;
					map.logAttack(id(), target->id(), _agility, hpAfter);
					return true;
				}
			}
			else
			{
				// Melee attack
				std::vector<core::Unit*> meleeTargets;
				for (auto* u : adjacent)
				{
					if (u->canBeAttacked() && u->canBeAttackedMelee())
					{
						meleeTargets.push_back(u);
					}
				}

				if (!meleeTargets.empty())
				{
					auto* target = map.pickRandomTarget(meleeTargets);
					target->changeHP(-_strength);
					uint32_t hpAfter = target->hp() > 0 ? static_cast<uint32_t>(target->hp()) : 0;
					map.logAttack(id(), target->id(), _strength, hpAfter);
					return true;
				}
			}

			if (hasTarget())
			{
				return map.moveUnitTowardsTarget(*this);
			}
			return false;
		}

	private:
		uint32_t _agility;
		uint32_t _strength;
		uint32_t _range;
	};
}
