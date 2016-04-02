#pragma once

#include "game-engine/common.h"
#include "buff-minion.h"
#include "impl/buff-minion.h"

namespace GameEngine
{
	inline Enchantment_BuffMinion::Enchantment_BuffMinion(int attack_boost, int hp_boost, int damage_spell_boost, int buff_flags, bool one_turn)
		: attack_boost(attack_boost), hp_boost(hp_boost), damage_spell_boost(damage_spell_boost),
		buff_flags(buff_flags), one_turn(one_turn)
	{
#ifdef DEBUG
		this->after_added_called = false;
#endif
	}

	inline bool Enchantment_BuffMinion::EqualsTo(Enchantment<Minion> const & rhs_base) const
	{
		auto rhs = dynamic_cast<decltype(this)>(&rhs_base);
		if (!rhs) return false;

		if (this->attack_boost != rhs->attack_boost) return false;
		if (this->hp_boost != rhs->hp_boost) return false;
		if (this->damage_spell_boost != rhs->damage_spell_boost) return false;
		if (this->buff_flags != rhs->buff_flags) return false;
		if (this->one_turn != rhs->one_turn) return false;
		if (this->actual_attack_boost != rhs->actual_attack_boost) return false;

		return true;
	}

	inline std::size_t Enchantment_BuffMinion::GetHash() const
	{
		std::size_t result = typeid(decltype(*this)).hash_code();

		GameEngine::hash_combine(result, this->attack_boost);
		GameEngine::hash_combine(result, this->hp_boost);
		GameEngine::hash_combine(result, this->damage_spell_boost);
		GameEngine::hash_combine(result, this->buff_flags);
		GameEngine::hash_combine(result, this->one_turn);
		GameEngine::hash_combine(result, this->actual_attack_boost);

		return result;
	}

	inline void Enchantment_BuffMinion::AfterAdded(Minion & minion)
	{
#ifdef DEBUG
		this->after_added_called = true;
#endif
		return Impl::Enchantment_BuffMinion::AfterAdded(this->attack_boost, this->hp_boost, this->damage_spell_boost, this->buff_flags, this->actual_attack_boost, minion);
	}

	inline void Enchantment_BuffMinion::BeforeRemoved(Minion & minion)
	{
#ifdef DEBUG
		if (this->after_added_called == false) throw std::runtime_error("AfterAdded() should be called before");
#endif
		return Impl::Enchantment_BuffMinion::BeforeRemoved(this->attack_boost, this->hp_boost, this->damage_spell_boost, this->buff_flags, this->actual_attack_boost, minion);
	}

	inline bool Enchantment_BuffMinion::TurnEnd(Minion & minion)
	{
		if (one_turn) return false; // one-turn effect 
		else return true;
	}

} // namespace GameEngine