#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-premium=1&filter-class=1&sort=-cost&display=1

namespace Cards
{
	struct Card_CS2_189 : public MinionCardBase<Card_CS2_189> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return TargetsGenerator(context.player_).Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Character(context.GetTarget())
				.Damage(context.card_ref_, 1);
		}
	};

	struct Card_CS1_042 : public MinionCardBase<Card_CS1_042, Taunt> {};

	struct Card_EX1_508o : public Enchantment<Card_EX1_508o, Attack<1>> {};
	struct Card_EX1_508 : public MinionCardBase<Card_EX1_508> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally(context).Minion().Murlocs() // friendly murlocs only
				.Exclude(context.card_ref_) // only apply on other murlocs
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_EX1_508() {
			Aura<Card_EX1_508o, EmitWhenAlive, UpdateWhenMinionChanged>();
		}
	};

	struct Card_CS2_171 : public MinionCardBase<Card_CS2_171, Charge> {};

	struct Card_EX1_011 : public MinionCardBase<Card_EX1_011> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return TargetsGenerator(context.player_).Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Character(context.GetTarget()).Heal(
				context.card_ref_, 2);
		}
	};

	struct Card_EX1_066 : public MinionCardBase<Card_EX1_066> {
		static void Battlecry(Contexts::OnPlay context) {
			Manipulate(context).OpponentHero().DestroyWeapon();
		}
	};

	struct Card_CS2_173 : public MinionCardBase<Card_CS2_173, Charge> {};
	struct Card_CS2_121 : public MinionCardBase<Card_CS2_121, Taunt> {};
	struct Card_CS2_142 : public MinionCardBase<Card_CS2_142, SpellDamage<1>> {};

	struct Card_EX1_506 : public MinionCardBase<Card_EX1_506> {
		static void Battlecry(Contexts::OnPlay context) {
			SummonToRight(context, Cards::ID_EX1_506a);
		}
	};

	struct Card_EX1_506a : public MinionCardBase<Card_EX1_506a> {};
	struct Card_EX1_015 : public MinionCardBase<Card_EX1_015> {
		static void Battlecry(Contexts::OnPlay context) {
			Manipulate(context).CurrentHero().DrawCard();
		}
	};

	struct Card_EX1_582 : public MinionCardBase<Card_EX1_582, SpellDamage<1>> {};
	struct Card_CS2_141 : public MinionCardBase<Card_CS2_141> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return TargetsGenerator(context.player_).Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Character(context.GetTarget()).Damage(context.card_ref_, 1);
		}
	};

	struct Card_CS2_125 : public MinionCardBase<Card_CS2_125, Taunt> {};
	struct Card_CS2_122e : public Enchantment<Card_CS2_122e, Attack<1>> {};

	struct Card_CS2_122 : public MinionCardBase<Card_CS2_122> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally(context).Minion() // friendly minions
				.Exclude(context.card_ref_) // only apply on other
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_CS2_122() {
			Aura<Card_CS2_122e, EmitWhenAlive, UpdateWhenMinionChanged>();
		}
	};

	struct Card_CS2_196 : public MinionCardBase<Card_CS2_196> {
		static void Battlecry(Contexts::OnPlay context) {
			SummonToRight(context, Cards::ID_CS2_boar);
		}
	};

	struct Card_EX1_019e : public Enchantment<Card_EX1_019e, MaxHP<1>, Attack<1>> {};
	struct Card_EX1_019 : public MinionCardBase<Card_EX1_019> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return TargetsGenerator(context.player_).Ally(context).Minion().Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			if (!context.GetTarget().IsValid()) return;
			Manipulate(context).Minion(context.GetTarget()).Enchant().Add<Card_EX1_019e>();
		}
	};

	struct Card_CS2_127 : public MinionCardBase<Card_CS2_127, Taunt> {};
	struct Card_CS2_124 : public MinionCardBase<Card_CS2_124, Charge> {};
	struct Card_EX1_025 : public MinionCardBase<Card_EX1_025> {
		static void Battlecry(Contexts::OnPlay context) {
			SummonToRight(context, Cards::ID_EX1_025t);
		}
	};

	struct Card_CS2_147 : public MinionCardBase<Card_CS2_147> {
		static void Battlecry(Contexts::OnPlay context) {
			Manipulate(context).CurrentHero().DrawCard();
		}
	};

	struct Card_CS2_197 : public MinionCardBase<Card_CS2_197, SpellDamage<1>> {};
	struct Card_CS2_179 : public MinionCardBase<Card_CS2_179, Taunt> {};
	struct Card_CS2_131 : public MinionCardBase<Card_CS2_131, Charge> {};
	struct Card_CS2_187 : public MinionCardBase<Card_CS2_187, Taunt> {};
	struct Card_DS1_055 : public MinionCardBase<Card_DS1_055> {
		static void Battlecry(Contexts::OnPlay context) {
			ForEach(context, Targets(context.player_).Ally(context).Exclude(context.card_ref_),
				[context](FlowControl::Manipulate manipulate, state::CardRef ref) {
				manipulate.Character(ref).Heal(context.card_ref_, 2);
			});
		}
	};

	template <int Arg1>
	struct Card_CS2_226e : public Enchantment<Card_CS2_226e<Arg1>, MaxHP<Arg1>,Attack<Arg1>> {};
	struct Card_CS2_226 : public MinionCardBase<Card_CS2_226> {
		static void Battlecry(Contexts::OnPlay context) {
			int count = 0;
			Targets(context.player_).Ally(context).Minion().Exclude(context.card_ref_).GetInfo()
				.Count(context.manipulate_, &count);

			switch (count) {
			case 0:
				break;
			case 1:
				Manipulate(context).Minion(context.card_ref_).Enchant().Add<Card_CS2_226e<1>>();
				break;
			case 2:
				Manipulate(context).Minion(context.card_ref_).Enchant().Add<Card_CS2_226e<2>>();
				break;
			case 3:
				Manipulate(context).Minion(context.card_ref_).Enchant().Add<Card_CS2_226e<3>>();
				break;
			case 4:
				Manipulate(context).Minion(context.card_ref_).Enchant().Add<Card_CS2_226e<4>>();
				break;
			case 5:
				Manipulate(context).Minion(context.card_ref_).Enchant().Add<Card_CS2_226e<5>>();
				break;
			case 6:
				Manipulate(context).Minion(context.card_ref_).Enchant().Add<Card_CS2_226e<6>>();
				break;
			default:
				throw std::exception("invalid minion count");
			}
		}
	};

	struct Card_EX1_399e : public Enchantment<Card_EX1_399e, Attack<3>> {};
	struct Card_EX1_399 : public MinionCardBase<Card_EX1_399> {
		template <typename Context>
		static bool HandleEvent(state::CardRef self, Context&& context) {
			Manipulate(context).Card(self).Enchant().Add<Card_EX1_399e>();
			return true;
		};

		Card_EX1_399() {
			RegisterEvent<OnSelfTakeDamage>();
		}
	};

	struct Card_EX1_593 : public MinionCardBase<Card_EX1_593> {
		static void Battlecry(Contexts::OnPlay context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier().Opposite();
			context.manipulate_.Hero(player).Damage(context.card_ref_, 3);
		}
	};

	struct Card_CS2_150 : public MinionCardBase<Card_CS2_150> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return TargetsGenerator(context.player_).Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Character(context.GetTarget()).Damage(context.card_ref_, 2);
		}
	};

	struct Card_CS2_155 : public MinionCardBase<Card_CS2_155, SpellDamage<1>> {};
	struct Card_CS2_162 : public MinionCardBase<Card_CS2_162, Taunt> {};
	struct Card_CS2_213 : public MinionCardBase<Card_CS2_213, Charge> {};

	struct Card_CS2_222o : public Enchantment<Card_CS2_222o, Attack<1>, MaxHP<1>> {};
	struct Card_CS2_222 : public MinionCardBase<Card_CS2_222> {
		// TODO: single aura target
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally(context).Minion() // friendly minions
				.Exclude(context.card_ref_) // only apply on other
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_CS2_222() {
			Aura<Card_CS2_222o, EmitWhenAlive, UpdateWhenMinionChanged>();
		}
	};
}

REGISTER_CARD(CS2_222)
REGISTER_CARD(CS2_213)
REGISTER_CARD(CS2_162)
REGISTER_CARD(CS2_155)
REGISTER_CARD(CS2_150)
REGISTER_CARD(EX1_593)
REGISTER_CARD(EX1_399)
REGISTER_CARD(CS2_226)
REGISTER_CARD(DS1_055)
REGISTER_CARD(CS2_187)
REGISTER_CARD(CS2_131)
REGISTER_CARD(CS2_179)
REGISTER_CARD(CS2_197)
REGISTER_CARD(CS2_147)
REGISTER_CARD(EX1_025)

REGISTER_CARD(CS2_189)
REGISTER_CARD(CS1_042)
REGISTER_CARD(EX1_508)
REGISTER_CARD(CS2_171)
REGISTER_CARD(EX1_011)
REGISTER_CARD(EX1_066)
REGISTER_CARD(CS2_173)
REGISTER_CARD(CS2_121)
REGISTER_CARD(CS2_142)
REGISTER_CARD(EX1_506)
REGISTER_CARD(EX1_506a)
REGISTER_CARD(EX1_015)
REGISTER_CARD(EX1_582)
REGISTER_CARD(CS2_141)
REGISTER_CARD(CS2_125)
REGISTER_CARD(CS2_122)
REGISTER_CARD(CS2_196)
REGISTER_CARD(EX1_019)
REGISTER_CARD(CS2_127)
REGISTER_CARD(CS2_124)
