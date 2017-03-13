#pragma once

#include "state/Types.h"

namespace state
{
	namespace Cards
	{
		class EnchantableStates
		{
		public:
			EnchantableStates() :
				cost(-1), attack(-1), max_hp(-1),
				taunt(false), shielded(false), charge(false),
				spell_damage(0)
			{
				static_assert(kFieldChangeId == 5, "field changed");
			}

			bool operator==(EnchantableStates const& rhs) const
			{
				static_assert(kFieldChangeId == 5, "field changed");
				if (player != rhs.player) return false;
				if (cost != rhs.cost) return false;
				if (attack != rhs.attack) return false;
				if (max_hp != rhs.max_hp) return false;
				if (taunt != rhs.taunt) return false;
				if (shielded != rhs.shielded) return false;
				if (charge != rhs.charge) return false;
				if (spell_damage != rhs.spell_damage) return false;
				return true;
			}

			bool operator!=(EnchantableStates const& rhs) const { return !(*this == rhs); }

		public:
			static constexpr int kFieldChangeId = 5; // Change this if any field is changed. This helps to see where you should also modify

			PlayerIdentifier player;
			int cost;
			int attack;
			int max_hp;

			bool taunt;
			bool shielded;
			bool charge;

			int spell_damage;
		};
	}
}