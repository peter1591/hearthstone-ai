#pragma once

#include "enchantment.h"

namespace GameEngine
{
	// Introduce some attack/hp/taunt/charge/etc. buffs on minion
	// buff_flags are ORed flags for MinionStat::Flag
	class Enchantment_BuffMinion : public Enchantment<Minion>
	{
	public:
		Enchantment_BuffMinion(int attack_boost, int hp_boost, int damage_spell_boost, int buff_flags, bool one_turn);
		bool EqualsTo(Enchantment<Minion> const& rhs_base) const;
		std::size_t GetHash() const;

	public:
		void Apply(Minion & minion);
		void Remove(Minion & minion);
		void TurnEnd(Minion & minion, bool & expired);

	private:
#ifdef DEBUG
		bool after_added_called;
#endif
		int attack_boost;
		int hp_boost;
		int damage_spell_boost;
		int buff_flags;
		bool one_turn;
		int actual_attack_boost; // attack cannot be negative
	};
} // GameEngine