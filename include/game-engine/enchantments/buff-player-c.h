#pragma once

#include "game-engine/enchantments/enchantment.h"

namespace GameEngine
{
	class Player;

	// Introduce buffs on player
	template <int spell_damage_boost, bool one_turn>
	class Enchantment_BuffPlayer_C : public Enchantment<Player>
	{
	public:
		bool EqualsTo(Enchantment<Player> const& rhs_base) const { return dynamic_cast<decltype(this)>(&rhs_base) != nullptr; }
		std::size_t GetHash() const { return typeid(decltype(*this)).hash_code(); }

	public:
		void Apply(Player & player);
		void Remove(Player & player);
		void TurnEnd(Player & player, bool & expired);
	};
} // namespace GameEngine