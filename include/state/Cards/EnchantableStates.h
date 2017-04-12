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
				cost(-1), cost_health_instead(false), attack(-1), max_hp(-1), charge(false), stealth(false), max_attacks_per_turn(1),
				immune_to_spell(false), poisonous(false), spell_damage(0)
			{
				static_assert(kFieldChangeId == 13, "field changed");
			}

			bool operator==(EnchantableStates const& rhs) const
			{
				static_assert(kFieldChangeId == 13, "field changed");
				if (player != rhs.player) return false;
				if (cost != rhs.cost) return false;
				if (cost_health_instead != rhs.cost_health_instead) return false;
				if (attack != rhs.attack) return false;
				if (max_hp != rhs.max_hp) return false;
				if (charge != rhs.charge) return false;
				if (stealth != rhs.stealth) return false;
				if (max_attacks_per_turn != rhs.max_attacks_per_turn) return false;
				if (immune_to_spell != rhs.immune_to_spell) return false;
				if (poisonous != rhs.poisonous) return false;
				if (spell_damage != rhs.spell_damage) return false;
				return true;
			}

			bool operator!=(EnchantableStates const& rhs) const { return !(*this == rhs); }

			void RestoreToSilenceDefault() {
				static_assert(kFieldChangeId == 13, "field changed");
				// only preserve cost/attack/hp
				cost_health_instead = false;
				charge = false;
				stealth = false;
				max_attacks_per_turn = 1;
				immune_to_spell = false;
				poisonous = false;
				spell_damage = 0;
			}

		public:
			static constexpr int kFieldChangeId = 13; // Change this if any field is changed. This helps to see where you should also modify

			PlayerIdentifier player;
			int cost;
			bool cost_health_instead; // TODO: respect this flag
			int attack;
			int max_hp;
			bool charge;
			bool stealth;
			int max_attacks_per_turn;
			bool immune_to_spell;
			bool poisonous;

			int spell_damage;
		};
	}
}