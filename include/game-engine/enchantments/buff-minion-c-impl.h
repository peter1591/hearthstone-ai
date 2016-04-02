#pragma once

#include "buff-minion-c.h"

namespace GameEngine {

	template<int attack_boost, int hp_boost, int spell_damage_boost, int buff_flags, bool one_turn>
	inline GameEngine::Enchantment_BuffMinion_C<attack_boost, hp_boost, spell_damage_boost, buff_flags, one_turn>::Enchantment_BuffMinion_C()
	{
#ifdef DEBUG
		this->after_added_called = false;
#endif
	}

	template<int attack_boost, int hp_boost, int spell_damage_boost, int buff_flags, bool one_turn>
	inline void Enchantment_BuffMinion_C<attack_boost, hp_boost, spell_damage_boost, buff_flags, one_turn>::AfterAdded(Minion & minion)
	{
#ifdef DEBUG
		this->after_added_called = true;
#endif
		return Impl::Enchantment_BuffMinion::AfterAdded<buff_flags>(attack_boost, hp_boost, spell_damage_boost, this->actual_attack_boost, minion);
	}

	template<int attack_boost, int hp_boost, int spell_damage_boost, int buff_flags, bool one_turn>
	inline void Enchantment_BuffMinion_C<attack_boost, hp_boost, spell_damage_boost, buff_flags, one_turn>::BeforeRemoved(Minion & minion)
	{
#ifdef DEBUG
		if (this->after_added_called == false) throw std::runtime_error("AfterAdded() should be called before");
#endif
		return Impl::Enchantment_BuffMinion::BeforeRemoved<buff_flags>(attack_boost, hp_boost, spell_damage_boost, this->actual_attack_boost, minion);
	}

	template<int attack_boost, int hp_boost, int spell_damage_boost, int buff_flags, bool one_turn>
	inline bool Enchantment_BuffMinion_C<attack_boost, hp_boost, spell_damage_boost, buff_flags, one_turn>::TurnEnd(Minion & minion)
	{
		if (one_turn) return false; // one-turn effect 
		else return true;
	}

	template<int attack_boost, int hp_boost, int spell_damage_boost, int buff_flags, bool one_turn>
	inline bool Enchantment_BuffMinion_C<attack_boost, hp_boost, spell_damage_boost, buff_flags, one_turn>::EqualsTo(Enchantment<Minion> const & rhs_base) const
	{
		auto rhs = dynamic_cast<decltype(this)>(&rhs_base);
		if (!rhs) return false;

		if (this->actual_attack_boost != rhs->actual_attack_boost) return false;

		return true;
	}

	template<int attack_boost, int hp_boost, int spell_damage_boost, int buff_flags, bool one_turn>
	inline std::size_t Enchantment_BuffMinion_C<attack_boost, hp_boost, spell_damage_boost, buff_flags, one_turn>::GetHash() const
	{
		std::size_t result = typeid(decltype(*this)).hash_code();
		GameEngine::hash_combine(result, this->actual_attack_boost);
		return result;
	}
} // namespace GameEngine