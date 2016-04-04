#pragma once

#include "game-engine/player.h"
#include "buff-player-c.h"

namespace GameEngine
{
	template <int spell_damage_boost, bool one_turn>
	void Enchantment_BuffPlayer_C<spell_damage_boost, one_turn>::Apply(Player & player)
	{
		Enchantment<Player>::Apply(player);
		player.stat.spell_damage += spell_damage_boost;

#ifdef DEBUG
		if (player.stat.spell_damage < 0) throw std::runtime_error("if spell damage might be reduced, we need to make sure the spell damage might be reduced below zero.");
#endif
	}

	template<int spell_damage_boost, bool one_turn>
	inline void Enchantment_BuffPlayer_C<spell_damage_boost, one_turn>::Remove(Player & player)
	{
		Enchantment<Player>::Remove(player);
		player.stat.spell_damage -= spell_damage_boost;
	}

	template<int spell_damage_boost, bool one_turn>
	inline void Enchantment_BuffPlayer_C<spell_damage_boost, one_turn>::TurnEnd(Player &, bool & expired)
	{
		if (one_turn) expired = true; // one-turn effect 
		else expired = false;
	}
} // namespace GameEngine