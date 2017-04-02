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
				cost(-1), attack(-1), max_hp(-1), charge(false), stealth(false), windfury(false), spell_damage(0)
			{
				static_assert(kFieldChangeId == 9, "field changed");
			}

			bool operator==(EnchantableStates const& rhs) const
			{
				static_assert(kFieldChangeId == 9, "field changed");
				if (player != rhs.player) return false;
				if (cost != rhs.cost) return false;
				if (attack != rhs.attack) return false;
				if (max_hp != rhs.max_hp) return false;
				if (charge != rhs.charge) return false;
				if (stealth != rhs.stealth) return false;
				if (windfury != rhs.windfury) return false;
				if (spell_damage != rhs.spell_damage) return false;
				return true;
			}

			bool operator!=(EnchantableStates const& rhs) const { return !(*this == rhs); }

		public:
			static constexpr int kFieldChangeId = 9; // Change this if any field is changed. This helps to see where you should also modify

			PlayerIdentifier player;
			int cost;
			int attack;
			int max_hp;
			bool charge;
			bool stealth;
			bool windfury;

			int spell_damage;
		};
	}
}