#pragma once

#include "Cards/MinionCardUtils.h"

// http://www.hearthpwn.com/cards?filter-set=2&filter-premium=1&filter-class=1&sort=-cost&display=1

namespace Cards
{
	struct Card_CS2_189 : public MinionCardBase<Card_CS2_189> {
		template <typename Context>
		static bool GetBattleTargets(state::targetor::TargetsGenerator& target, Context&& context) {
			target.Targetable();
			return true;
		}

		Card_CS2_189() {
			Battlecry<WithTargets>([](auto context) {
				Damage(context).Target(context.flow_context_.battlecry_target_).Amount(1);
			});
		}
	};

	struct Card_CS1_042 : public MinionCardBase<Card_CS1_042> {
		Card_CS1_042() { Taunt(); }
	};

	struct Card_CS2_168 : public MinionCardBase<Card_CS2_168> {};

	struct Card_EX1_508o : public EnchantmentCardBase {
		static constexpr EnchantmentTiers tier = kEnchantmentAura;
		static constexpr int id = Cards::ID_EX1_508o;

		Card_EX1_508o() {
			apply_functor = [](auto& stats) {
				++stats.attack;
			};
		}
	};

	struct Card_EX1_508 : public MinionCardBase<Card_EX1_508> {
		template <typename Context>
		static bool GetAuraTargets(state::targetor::TargetsGenerator& target, Context&& context) {
			target
				.Ally(context).Minion().Murlocs() // friendly murlocs only
				.Exclude(context.card_ref_); // only apply on other murlocs
			return true;
		}
		Card_EX1_508() {
			Aura<Card_EX1_508o, EmitWhenAlive, UpdateWhenMinionChanged>();
		}
	};

	struct Card_CS2_171 : public MinionCardBase<Card_CS2_171> {
		Card_CS2_171() { Charge(); }
	};

	struct Card_EX1_011 : public MinionCardBase<Card_EX1_011> {
		template <typename Context>
		static bool GetBattleTargets(state::targetor::TargetsGenerator& target, Context&& context) {
			target.Targetable();
			return true;
		}

		Card_EX1_011() {
			Battlecry<WithTargets>([](auto context) {
				Heal(context).Target(context.flow_context_.battlecry_target_).Amount(2);
			});
		}
	};

	struct Card_EX1_066 : public MinionCardBase<Card_EX1_066> {
		Card_EX1_066() {
			Battlecry<NoTarget>([](auto context) {
				Manipulate(context).OpponentHero().DestroyWeapon();
			});
		}
	};

	struct Card_CS2_172 : public MinionCardBase<Card_CS2_172> {};

	struct Card_CS2_173 : public MinionCardBase<Card_CS2_173> {
		Card_CS2_173() { Charge(); }
	};

	struct Card_CS2_121 : public MinionCardBase<Card_CS2_121>{
		Card_CS2_121() { Taunt(); }
	};

	struct Card_CS2_142 : public MinionCardBase<Card_CS2_142> {
		Card_CS2_142() { SpellDamage(1); }
	};

	struct Card_EX1_506 : public MinionCardBase<Card_EX1_506> {
		Card_EX1_506() {
			Battlecry<NoTarget>([](auto context) {
				SummonToRight(context, Cards::ID_EX1_506a);
			});
		}
	};

	struct Card_EX1_506a : public MinionCardBase<Card_EX1_506a> {
	};

	struct Card_EX1_015 : public MinionCardBase<Card_EX1_015> {
		Card_EX1_015() {
			Battlecry<NoTarget>([](auto context) {
				Manipulate(context).CurrentHero().DrawCard();
			});
		}
	};

	struct Card_CS2_120 : public MinionCardBase<Card_CS2_120> {
	};

	struct Card_EX1_582 : public MinionCardBase<Card_EX1_582> {
		Card_EX1_582() { SpellDamage(1); }
	};

	struct Card_CS2_141 : public MinionCardBase<Card_CS2_141> {
		template <typename Context>
		static bool GetBattleTargets(state::targetor::TargetsGenerator& target, Context&& context) {
			target.Targetable();
			return true;
		}

		Card_CS2_141() {
			Battlecry<WithTargets>([](auto context) {
				Damage(context).Target(context.flow_context_.battlecry_target_).Amount(1);
			});
		}
	};

	struct Card_CS2_125 : public MinionCardBase<Card_CS2_125> {
		Card_CS2_125() { Taunt(); }
	};

	struct Card_CS2_118 : public MinionCardBase<Card_CS2_118> {
	};

	struct Card_CS2_122e : public EnchantmentCardBase {
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;
		static constexpr int id = Cards::ID_CS2_122e;

		Card_CS2_122e() {
			apply_functor = [](auto& stats) {
				++stats.attack;
			};
		}
	};

	struct Card_CS2_122 : public MinionCardBase<Card_CS2_122> {
		template <typename Context>
		static void GetAuraTargets(state::targetor::TargetsGenerator& targets, Context&& context) {
			targets
				.Ally(context).Minion() // friendly minions
				.Exclude(context.card_ref_); // only apply on other
		}
		Card_CS2_122() {
			Aura<Card_CS2_122e, EmitWhenAlive, UpdateWhenMinionChanged>();
		}
	};

	struct Card_CS2_boar : public MinionCardBase<Card_CS2_boar> {
	};

	struct Card_CS2_196 : public MinionCardBase<Card_CS2_196> {
		Card_CS2_196() {
			Battlecry<NoTarget>([](auto context) {
				SummonToRight(context, Cards::ID_CS2_boar);
			});
		}
	};

	struct Card_EX1_019e : public EnchantmentCardBase {
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;
		static constexpr int id = Cards::ID_EX1_019e;

		Card_EX1_019e() {
			apply_functor = [](auto& stats) {
				++stats.max_hp;
				++stats.attack;
			};
		}
	};

	struct Card_EX1_019 : public MinionCardBase<Card_EX1_019> {
		template <typename Context>
		static bool GetBattleTargets(state::targetor::TargetsGenerator& target, Context&& context) {
			target.Ally(context).Minion().Targetable();
			return true;
		}

		Card_EX1_019() {
			Battlecry<WithTargets>([](auto context) {
				Manipulate(context).Minion(context.flow_context_.battlecry_target_)
					.Enchant().Add(Card_EX1_019e());
			});
		}
	};

	struct Card_CS2_127 : public MinionCardBase<Card_CS2_127> {
		Card_CS2_127() { Taunt(); }
	};

	struct Card_CS2_124 : public MinionCardBase<Card_CS2_124> {
		Card_CS2_124() { Charge(); }
	};

	struct Card_CS2_182 : public MinionCardBase<Card_CS2_182> {
	};

	struct Card_EX1_025t : public MinionCardBase<Card_EX1_025t> {
	};

	struct Card_EX1_025 : public MinionCardBase<Card_EX1_025> {
		Card_EX1_025() {
			Battlecry<NoTarget>([](auto context) {
				SummonToRight(context, Cards::ID_EX1_025t);
			});
		}
	};

	struct Card_CS2_147 : public MinionCardBase<Card_CS2_147> {
		Card_CS2_147() {
			Battlecry<NoTarget>([](auto context) {
				Manipulate(context).CurrentHero().DrawCard();
			});
		}
	};

	struct Card_CS2_119 : public MinionCardBase<Card_CS2_119>{
	};

	struct Card_CS2_197 : public MinionCardBase<Card_CS2_197> {
		Card_CS2_197() { SpellDamage(1); }
	};

	struct Card_CS2_179 : public MinionCardBase<Card_CS2_179> {
		Card_CS2_179() { Taunt(); }
	};

	struct Card_CS2_131 : public MinionCardBase<Card_CS2_131> {
		Card_CS2_131() { Charge(); }
	};

	struct Card_CS2_187 : public MinionCardBase<Card_CS2_187>{
		Card_CS2_187() { Taunt(); }
	};

	struct Card_DS1_055 : public MinionCardBase<Card_DS1_055> {
		Card_DS1_055() {
			Battlecry<NoTarget>([](auto context) {
				ForEach(context, Targets().Ally(context).Exclude(context.card_ref_),
					[](state::State & state, FlowControl::FlowContext & flow_context, state::CardRef ref) {
					FlowControl::Manipulate(state, flow_context).Character(ref).Heal(2);
				});
			});
		}
	};

	template <int Arg1>
	struct Card_CS2_226e : public EnchantmentCardBase {
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;
		static constexpr int id = Cards::ID_EX1_019e;

		Card_CS2_226e() {
			apply_functor = [](auto& stats) {
				stats.max_hp += Arg1;
				stats.attack += Arg1;
			};
		}
	};

	struct Card_CS2_226 : public MinionCardBase<Card_CS2_226> {
		Card_CS2_226() {
			Battlecry<NoTarget>([](auto context) {
				int count = 0;
				Targets().Ally(context).Minion().Exclude(context.card_ref_).GetInfo()
					.Count(context.state_, &count);

				switch (count) {
				case 0:
					break;
				case 1:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<1>());
					break;
				case 2:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<2>());
					break;
				case 3:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<3>());
					break;
				case 4:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<4>());
					break;
				case 5:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<5>());
					break;
				case 6:
					Manipulate(context).Minion(context.card_ref_).Enchant().Add(Card_CS2_226e<6>());
					break;
				default:
					throw std::exception("invalid minion count");
				}
			});
		}
	};

	struct Card_EX1_399e : public EnchantmentCardBase {
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;
		static constexpr int id = Cards::ID_EX1_399e;

		Card_EX1_399e() {
			apply_functor = [](auto& stats) {
				stats.attack += 3;
			};
		}
	};

	struct Card_EX1_399 : public MinionCardBase<Card_EX1_399> {
		template <typename Context>
		static bool HandleEvent(state::CardRef self, Context&& context) {
			Manipulate(context).Card(self).Enchant().Add(Card_EX1_399e());
			return true;
		};

		Card_EX1_399() {
			RegisterEvent<OnSelfTakeDamage>();
		}
	};

	struct Card_EX1_593 : public MinionCardBase<Card_EX1_593> {
		Card_EX1_593() {
			Battlecry<NoTarget>([](auto context) {
				Damage(context).Opponent().Amount(3);
			});
		}
	};

	struct Card_CS2_150 : public MinionCardBase<Card_CS2_150> {
		template <typename Context>
		static bool GetBattleTargets(state::targetor::TargetsGenerator& target, Context&& context) {
			target.Targetable();
			return true;
		}

		Card_CS2_150() {
			Battlecry<WithTargets>([](auto context) {
				Damage(context).Target(context.flow_context_.battlecry_target_).Amount(2);
			});
		}
	};

	struct Card_CS2_155 : public MinionCardBase<Card_CS2_155> {
		Card_CS2_155() { SpellDamage(1); }
	};

	struct Card_CS2_200 : public MinionCardBase<Card_CS2_200> {
	};

	struct Card_CS2_162 : public MinionCardBase<Card_CS2_162> {
		Card_CS2_162() { Taunt(); }
	};

	struct Card_CS2_213 : public MinionCardBase<Card_CS2_213> {
		Card_CS2_213() { Charge(); }
	};

	struct Card_CS2_201 : public MinionCardBase<Card_CS2_201> {
	};

	struct Card_CS2_222o : public EnchantmentCardBase {
		static constexpr EnchantmentTiers tier = kEnchantmentAura;
		static constexpr int id = Cards::ID_CS2_222o;

		Card_CS2_222o() {
			apply_functor = [](auto& stats) {
				++stats.attack;
				++stats.max_hp;
			};
		}
	};

	struct Card_CS2_222 : public MinionCardBase<Card_CS2_222> {
		template <typename Context> static void GetAuraTargets(state::targetor::TargetsGenerator& targets, Context&& context) {
			targets
				.Ally(context).Minion() // friendly minions
				.Exclude(context.card_ref_); // only apply on other
		}
		Card_CS2_222() {
			Aura<Card_CS2_222o, EmitWhenAlive, UpdateWhenMinionChanged>();
		}
	};

	struct Card_CS2_186 : public MinionCardBase<Card_CS2_186> {
	};
}


REGISTER_MINION_CARD(CS2_186)
REGISTER_MINION_CARD(CS2_222)
REGISTER_MINION_CARD(CS2_201)
REGISTER_MINION_CARD(CS2_213)
REGISTER_MINION_CARD(CS2_162)
REGISTER_MINION_CARD(CS2_200)
REGISTER_MINION_CARD(CS2_155)
REGISTER_MINION_CARD(CS2_150)
REGISTER_MINION_CARD(EX1_593)
REGISTER_MINION_CARD(EX1_399)
REGISTER_MINION_CARD(CS2_226)
REGISTER_MINION_CARD(DS1_055)
REGISTER_MINION_CARD(CS2_187)
REGISTER_MINION_CARD(CS2_131)
REGISTER_MINION_CARD(CS2_179)
REGISTER_MINION_CARD(CS2_197)
REGISTER_MINION_CARD(CS2_119)
REGISTER_MINION_CARD(CS2_147)
REGISTER_MINION_CARD(EX1_025t)
REGISTER_MINION_CARD(EX1_025)

REGISTER_MINION_CARD(CS2_189)
REGISTER_MINION_CARD(CS1_042)
REGISTER_MINION_CARD(CS2_168)
REGISTER_MINION_CARD(EX1_508)
REGISTER_MINION_CARD(CS2_171)
REGISTER_MINION_CARD(EX1_011)
REGISTER_MINION_CARD(EX1_066)
REGISTER_MINION_CARD(CS2_172)
REGISTER_MINION_CARD(CS2_173)
REGISTER_MINION_CARD(CS2_121)
REGISTER_MINION_CARD(CS2_142)
REGISTER_MINION_CARD(EX1_506)
REGISTER_MINION_CARD(EX1_506a)
REGISTER_MINION_CARD(EX1_015)
REGISTER_MINION_CARD(CS2_120)
REGISTER_MINION_CARD(EX1_582)
REGISTER_MINION_CARD(CS2_141)
REGISTER_MINION_CARD(CS2_125)
REGISTER_MINION_CARD(CS2_118)
REGISTER_MINION_CARD(CS2_122)
REGISTER_MINION_CARD(CS2_boar)
REGISTER_MINION_CARD(CS2_196)
REGISTER_MINION_CARD(EX1_019)
REGISTER_MINION_CARD(CS2_127)
REGISTER_MINION_CARD(CS2_124)
REGISTER_MINION_CARD(CS2_182)
