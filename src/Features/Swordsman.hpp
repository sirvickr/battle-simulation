#pragma once

#include <Core/GameMap.hpp>
#include <Core/Unit.hpp>

namespace sw::features
{
	class Swordsman : public core::Unit
	{
	public:
		Swordsman(uint32_t id, core::Position pos, int32_t hp, uint32_t strength) :
				core::Unit(id, pos, hp, 1),
				_strength(strength)
		{}

		bool act(core::GameMap& map) override
		{
			auto adjacent = map.findAdjacentUnits(position(), this);
			std::vector<core::Unit*> targets;
			for (auto* u : adjacent)
			{
				if (u->canBeAttacked() && u->canBeAttackedMelee())
				{
					targets.push_back(u);
				}
			}

			if (!targets.empty())
			{
				auto* target = map.pickRandomTarget(targets);
				target->changeHP(-_strength);
				uint32_t hpAfter = target->hp() > 0 ? static_cast<uint32_t>(target->hp()) : 0;
				map.logAttack(id(), target->id(), _strength, hpAfter);
				return true;
			}

			if (hasTarget())
			{
				return map.moveUnitTowardsTarget(*this);
			}

			return false;
		}

	private:
		uint32_t _strength;
	};
}
