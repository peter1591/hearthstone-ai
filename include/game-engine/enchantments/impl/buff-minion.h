#pragma once

#include "game-engine/board-objects/minion.h"

namespace GameEngine {
	namespace Impl {
		class Enchantment_BuffMinion {
		public:
			template <int buff_flags>
			static void AfterAdded(int attack_boost, int hp_boost, int spell_damage_boost, int &actual_attack_boost, Minion & minion)
			{
				AfterAdded_Stats(attack_boost, hp_boost, spell_damage_boost, actual_attack_boost, minion);
				SetMinionFlags<buff_flags>(minion, true);
			}
			static void AfterAdded(int attack_boost, int hp_boost, int spell_damage_boost, int buff_flags, int &actual_attack_boost, Minion & minion)
			{
				AfterAdded_Stats(attack_boost, hp_boost, spell_damage_boost, actual_attack_boost, minion);
				SetMinionFlags(buff_flags, minion, true);
			}

			template <int buff_flags>
			static void BeforeRemoved(int attack_boost, int hp_boost, int spell_damage_boost, int actual_attack_boost, Minion & minion)
			{
				BeforeRemoved_Stats(attack_boost, hp_boost, spell_damage_boost, actual_attack_boost, minion);
				SetMinionFlags<buff_flags>(minion, false);
			}

			static void BeforeRemoved(int attack_boost, int hp_boost, int spell_damage_boost, int buff_flags, int actual_attack_boost, Minion & minion)
			{
				BeforeRemoved_Stats(attack_boost, hp_boost, spell_damage_boost, actual_attack_boost, minion);
				SetMinionFlags(buff_flags, minion, false);
			}

		private:
			static void AfterAdded_Stats(int attack_boost, int hp_boost, int spell_damage_boost, int &actual_attack_boost, Minion & minion)
			{
				// attack cannot below to zero
				if (minion.GetAttack() + attack_boost < 0) {
					actual_attack_boost = -minion.GetAttack();
				}
				else { 
					actual_attack_boost = attack_boost;
				}
				if (actual_attack_boost != 0) {
					minion.AddAttack(actual_attack_boost);
				}

				if (hp_boost != 0) {
					minion.IncreaseCurrentAndMaxHP(hp_boost);
				}

				if (spell_damage_boost != 0) {
					minion.AddSpellDamage(spell_damage_boost);
				}
			}

			static void BeforeRemoved_Stats(int attack_boost, int hp_boost, int spell_damage_boost, int actual_attack_boost, Minion & minion)
			{
				if (actual_attack_boost != 0) {
					minion.AddAttack(-actual_attack_boost);
				}

				if (hp_boost != 0) {
					minion.DecreaseMaxHP(hp_boost);
				}

				if (spell_damage_boost != 0) {
					minion.AddSpellDamage(-spell_damage_boost);
				}
			}

			static void AlternateStatFlag(Minion & minion, MinionStat::Flag const& flag, bool val)
			{
				if (val) {
					minion.SetMinionStatFlag(flag);
				}
				else {
					minion.RemoveMinionStatFlag(flag);
				}
			}

			template <int buff_flags, int alternating_flag>
			class MinionFlagsSetter {
			public:
				static void SetFlag(Minion & minion, bool val) {
					constexpr int shifted_flag = 1 << alternating_flag;
					constexpr bool alternating = (buff_flags & shifted_flag) != 0;

					if (alternating) {
						AlternateStatFlag(minion, (MinionStat::Flag)alternating_flag, val);
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
						AlternateStatFlag(minion, (MinionStat::Flag)alternating_flag, val);
					}
				}
			}
		};
	} // namespace Impl
} // namespace GameEngine