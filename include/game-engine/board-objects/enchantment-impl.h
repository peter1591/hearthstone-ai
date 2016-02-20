#pragma once

#include "enchantment.h"

namespace GameEngine {
	namespace BoardObjects {

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline GameEngine::BoardObjects::Enchantment_BuffMinion<attack_boost, hp_boost, buff_flags, one_turn>::Enchantment_BuffMinion()
		{
#ifdef DEBUG
			this->after_added_called = false;
#endif
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline void Enchantment_BuffMinion<attack_boost, hp_boost, buff_flags, one_turn>::AfterAdded(Minion & minion)
		{
#ifdef DEBUG
			this->after_added_called = true;
#endif

			// attack cannot below to zero
			if (minion.GetAttack() + attack_boost < 0)
			{
				this->actual_attack_boost = -minion.GetAttack();
			}
			else {
				this->actual_attack_boost = attack_boost;
			}

			if (this->actual_attack_boost != 0)
			{
				minion.AddAttack(this->actual_attack_boost);
			}

			if (hp_boost != 0)
			{
				minion.IncreaseCurrentAndMaxHP(hp_boost);
			}

			this->SetMinionFlags(minion, true);
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline void Enchantment_BuffMinion<attack_boost, hp_boost, buff_flags, one_turn>::BeforeRemoved(Minion & minion)
		{
#ifdef DEBUG
			if (this->after_added_called == false) throw std::runtime_error("AfterAdded() should be called before");
#endif

			if (this->actual_attack_boost != 0)
			{
				minion.AddAttack(-this->actual_attack_boost);
			}

			if (hp_boost != 0)
			{
				minion.DecreaseMaxHP(hp_boost);
			}

			this->SetMinionFlags(minion, false);
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline bool Enchantment_BuffMinion<attack_boost, hp_boost, buff_flags, one_turn>::TurnEnd(Minion & minion)
		{
			if (one_turn) return false; // one-turn effect 
			else return true;
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline bool Enchantment_BuffMinion<attack_boost, hp_boost, buff_flags, one_turn>::EqualsTo(Enchantment const & rhs_base) const
		{
			auto rhs = dynamic_cast<decltype(this)>(&rhs_base);
			if (!rhs) return false;

			if (this->actual_attack_boost != rhs->actual_attack_boost) return false;

			return true;
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline std::size_t Enchantment_BuffMinion<attack_boost, hp_boost, buff_flags, one_turn>::GetHash() const
		{
			std::size_t result = std::hash<int>()(Enchantment::UniqueIdForHash::TypeBuffMinion);

			GameEngine::hash_combine(result, this->actual_attack_boost);
			GameEngine::hash_combine(result, hp_boost);
			GameEngine::hash_combine(result, buff_flags);
			GameEngine::hash_combine(result, one_turn);

			return result;
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline void Enchantment_BuffMinion<attack_boost, hp_boost, buff_flags, one_turn>::SetMinionFlags(Minion & minion, bool val)
		{
			MinionFlagsSetter<MinionStat::FLAG_MAX - 1>::SetFlag(minion, val);
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		template<int alternating_flag>
		inline void Enchantment_BuffMinion<attack_boost, hp_boost, buff_flags, one_turn>::MinionFlagsSetter<alternating_flag>::SetFlag(Minion & minion, bool val) 
		{
			constexpr int shifted_flag = 1 << alternating_flag;
			constexpr bool alternating = (buff_flags & shifted_flag) != 0;

			if (alternating) {
				minion.SetMinionStatFlag((MinionStat::Flag)alternating_flag, val);
			}

			MinionFlagsSetter<alternating_flag - 1>::SetFlag(minion, val);
		}
	}
}