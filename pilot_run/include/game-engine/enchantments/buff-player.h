#pragma once

#include <typeinfo>
#include "game-engine/common.h"
#include "game-engine/enchantments/enchantment.h"

namespace GameEngine
{
	class Player;

	// Introduce buffs on player
	class Enchantment_BuffPlayer : public Enchantment<Player>
	{
	public:
		Enchantment_BuffPlayer(int attack_boost, int spell_damage_boost, bool one_turn) :
			attack_boost(attack_boost), spell_damage_boost(spell_damage_boost), one_turn(one_turn)
		{}

		bool EqualsTo(Enchantment<Player> const& rhs_base) const {
			auto rhs = dynamic_cast<decltype(this)>(&rhs_base);
			if (!rhs) return false;

			if (this->attack_boost != rhs->attack_boost) return false;
			if (this->spell_damage_boost != rhs->spell_damage_boost) return false;
			if (this->one_turn != rhs->one_turn) return false;
			return true;
		}

		std::size_t GetHash() const {
			std::size_t result = typeid(decltype(*this)).hash_code();
			GameEngine::hash_combine(result, this->attack_boost);
			GameEngine::hash_combine(result, this->spell_damage_boost);
			GameEngine::hash_combine(result, this->one_turn);
			return result;
		}

	public:
		void Apply(Player & player);
		void Remove(Player & player);
		void TurnEnd(Player & player, bool & expired);

	private:
		int attack_boost;
		int spell_damage_boost;
		bool one_turn;
	};
} // namespace GameEngine