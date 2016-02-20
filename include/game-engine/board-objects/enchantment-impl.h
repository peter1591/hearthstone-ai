#pragma once

#include "enchantment.h"

namespace GameEngine {
	namespace BoardObjects {
		namespace Impl {
			class Enchantment_BuffMinion {
			public:
				template <int buff_flags>
				static void AfterAdded(int attack_boost, int hp_boost, int &actual_attack_boost, Minion & minion)
				{
					AfterAdded_AttackHP(attack_boost, hp_boost, actual_attack_boost, minion);
					SetMinionFlags<buff_flags>(minion, true);
				}
				static void AfterAdded(int attack_boost, int hp_boost, int buff_flags, int &actual_attack_boost, Minion & minion)
				{
					AfterAdded_AttackHP(attack_boost, hp_boost, actual_attack_boost, minion);
					SetMinionFlags(buff_flags, minion, true);
				}

				template <int buff_flags>
				static void BeforeRemoved(int attack_boost, int hp_boost, int actual_attack_boost, Minion & minion)
				{
					BeforeRemoved_AttackHP(attack_boost, hp_boost, actual_attack_boost, minion);
					SetMinionFlags<buff_flags>(minion, false);
				}

				static void BeforeRemoved(int attack_boost, int hp_boost, int buff_flags, int actual_attack_boost, Minion & minion)
				{
					BeforeRemoved_AttackHP(attack_boost, hp_boost, actual_attack_boost, minion);
					SetMinionFlags(buff_flags, minion, false);
				}

			private:
				static void BeforeRemoved_AttackHP(int attack_boost, int hp_boost, int actual_attack_boost, Minion & minion)
				{
					if (actual_attack_boost != 0)
					{
						minion.AddAttack(-actual_attack_boost);
					}

					if (hp_boost != 0)
					{
						minion.DecreaseMaxHP(hp_boost);
					}
				}

				template <int buff_flags, int alternating_flag>
				class MinionFlagsSetter {
				public:
					static void SetFlag(Minion & minion, bool val) {
						constexpr int shifted_flag = 1 << alternating_flag;
						constexpr bool alternating = (buff_flags & shifted_flag) != 0;

						if (alternating) {
							minion.SetMinionStatFlag((MinionStat::Flag)alternating_flag, val);
						}

						MinionFlagsSetter<buff_flags, alternating_flag - 1>::SetFlag(minion, val);
					}
				};

				template<int buff_flags>
				class MinionFlagsSetter<buff_flags, -1> {
				public:
					static void SetFlag(Minion & minion, bool val) { return; }
				};

				template <int buff_flags>
				static void SetMinionFlags(Minion & minion, bool val) {
					MinionFlagsSetter<buff_flags, MinionStat::FLAG_MAX - 1>::SetFlag(minion, val);
				}

				static void SetMinionFlags(int buff_flags, Minion & minion, bool val) {
					if (buff_flags == 0) return;

					for (int alternating_flag = 0; alternating_flag < MinionStat::FLAG_MAX; ++alternating_flag) {
						int shifted_flag = 1 << alternating_flag;
						bool alternating = (buff_flags & shifted_flag) != 0;

						if (alternating) {
							minion.SetMinionStatFlag((MinionStat::Flag)alternating_flag, val);
						}
					}
				}

				static void AfterAdded_AttackHP(int attack_boost, int hp_boost, int &actual_attack_boost, Minion & minion)
				{
					// attack cannot below to zero
					if (minion.GetAttack() + attack_boost < 0)
					{
						actual_attack_boost = -minion.GetAttack();
					}
					else {
						actual_attack_boost = attack_boost;
					}

					if (actual_attack_boost != 0)
					{
						minion.AddAttack(actual_attack_boost);
					}

					if (hp_boost != 0)
					{
						minion.IncreaseCurrentAndMaxHP(hp_boost);
					}
				}
			};
		}

		inline Enchantment_BuffMinion::Enchantment_BuffMinion(int attack_boost, int hp_boost, int buff_flags, bool one_turn)
			: attack_boost(attack_boost), hp_boost(hp_boost), buff_flags(buff_flags), one_turn(one_turn)
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
			if (this->buff_flags != rhs->buff_flags) return false;
			if (this->one_turn != rhs->one_turn) return false;
			if (this->actual_attack_boost != rhs->actual_attack_boost) return false;

			return true;
		}

		inline std::size_t Enchantment_BuffMinion::GetHash() const
		{
			std::size_t result = std::hash<int>()(Enchantment::UniqueIdForHash::TypeBuffMinion);

			GameEngine::hash_combine(result, this->attack_boost);
			GameEngine::hash_combine(result, this->hp_boost);
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
			return Impl::Enchantment_BuffMinion::AfterAdded(this->attack_boost, this->hp_boost, this->buff_flags, this->actual_attack_boost, minion);
		}

		inline void Enchantment_BuffMinion::BeforeRemoved(Minion & minion)
		{
#ifdef DEBUG
			if (this->after_added_called == false) throw std::runtime_error("AfterAdded() should be called before");
#endif
			return Impl::Enchantment_BuffMinion::BeforeRemoved(this->attack_boost, this->hp_boost, this->buff_flags, this->actual_attack_boost, minion);
		}

		inline bool Enchantment_BuffMinion::TurnEnd(Minion & minion)
		{
			if (one_turn) return false; // one-turn effect 
			else return true;
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline GameEngine::BoardObjects::Enchantment_BuffMinion_C<attack_boost, hp_boost, buff_flags, one_turn>::Enchantment_BuffMinion_C()
		{
#ifdef DEBUG
			this->after_added_called = false;
#endif
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline void Enchantment_BuffMinion_C<attack_boost, hp_boost, buff_flags, one_turn>::AfterAdded(Minion & minion)
		{
#ifdef DEBUG
			this->after_added_called = true;
#endif
			return Impl::Enchantment_BuffMinion::AfterAdded<buff_flags>(attack_boost, hp_boost, this->actual_attack_boost, minion);
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline void Enchantment_BuffMinion_C<attack_boost, hp_boost, buff_flags, one_turn>::BeforeRemoved(Minion & minion)
		{
#ifdef DEBUG
			if (this->after_added_called == false) throw std::runtime_error("AfterAdded() should be called before");
#endif
			return Impl::Enchantment_BuffMinion::BeforeRemoved<buff_flags>(attack_boost, hp_boost, this->actual_attack_boost, minion);
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline bool Enchantment_BuffMinion_C<attack_boost, hp_boost, buff_flags, one_turn>::TurnEnd(Minion & minion)
		{
			if (one_turn) return false; // one-turn effect 
			else return true;
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline bool Enchantment_BuffMinion_C<attack_boost, hp_boost, buff_flags, one_turn>::EqualsTo(Enchantment<Minion> const & rhs_base) const
		{
			auto rhs = dynamic_cast<decltype(this)>(&rhs_base);
			if (!rhs) return false;

			if (this->actual_attack_boost != rhs->actual_attack_boost) return false;

			return true;
		}

		template<int attack_boost, int hp_boost, int buff_flags, bool one_turn>
		inline std::size_t Enchantment_BuffMinion_C<attack_boost, hp_boost, buff_flags, one_turn>::GetHash() const
		{
			std::size_t result = std::hash<int>()(Enchantment::UniqueIdForHash::TypeBuffMinion_C);

			GameEngine::hash_combine(result, this->actual_attack_boost);
			GameEngine::hash_combine(result, hp_boost);
			GameEngine::hash_combine(result, buff_flags);
			GameEngine::hash_combine(result, one_turn);

			return result;
		}
	}
}