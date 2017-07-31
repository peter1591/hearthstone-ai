#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-premium=1&filter-class=1&sort=-cost&display=1
// Done

namespace Cards
{
	struct Card_GAME_005 : public SpellCardBase<Card_GAME_005> {
		Card_GAME_005() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int old_current = context.manipulate_.Board().Player(context.player_).GetResource().GetCurrent();
				context.manipulate_.Board().Player(context.player_).GetResource().SetCurrent(old_current + 1);
			});
		}
	};

	struct Card_CS2_189 : public MinionCardBase<Card_CS2_189> {
		static bool GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetOptionalBattlecryTargets(context.player_);
			return true;
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.OnBoardCharacter(target)
				.Damage(context.card_ref_, 1);
		}
	};

	struct Card_CS1_042 : public MinionCardBase<Card_CS1_042, Taunt> {};

	struct Card_EX1_508o : public Enchantment<Card_EX1_508o, Attack<1>> {};
	struct Card_EX1_508 : public MinionCardBase<Card_EX1_508> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets const& context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally().Minion().Murlocs() // friendly murlocs only
				.Exclude(context.card_ref_) // only apply on other murlocs
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_EX1_508() {
			Aura<Card_EX1_508o, EmitWhenAlive, FlowControl::aura::kUpdateWhenMinionChanges>();
		}
	};

	struct Card_CS2_171 : public MinionCardBase<Card_CS2_171, Charge> {};

	struct Card_EX1_011 : public MinionCardBase<Card_EX1_011> {
		static bool GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetRequiredBattlecryTargets(context.player_);
			return true;
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.OnBoardCharacter(target).Heal(
				context.card_ref_, 2);
		}
	};

	struct Card_EX1_066 : public MinionCardBase<Card_EX1_066> {
		static void Battlecry(Contexts::OnPlay const& context) {
			context.manipulate_.Player(context.player_.Opposite()).DestroyWeapon();
		}
	};

	struct Card_CS2_173 : public MinionCardBase<Card_CS2_173, Charge> {};
	struct Card_CS2_121 : public MinionCardBase<Card_CS2_121, Taunt> {};
	struct Card_CS2_142 : public MinionCardBase<Card_CS2_142, SpellDamage<1>> {};

	struct Card_EX1_506 : public MinionCardBase<Card_EX1_506> {
		static void Battlecry(Contexts::OnPlay const& context) {
			SummonToRight(context.manipulate_, context.card_ref_, Cards::ID_EX1_506a);
		}
	};

	struct Card_EX1_015 : public MinionCardBase<Card_EX1_015> {
		static void Battlecry(Contexts::OnPlay const& context) {
			context.manipulate_.Player(context.player_).DrawCard();
		}
	};

	struct Card_EX1_582 : public MinionCardBase<Card_EX1_582, SpellDamage<1>> {};
	struct Card_CS2_141 : public MinionCardBase<Card_CS2_141> {
		static bool GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetOptionalBattlecryTargets(context.player_);
			return true;
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 1);
		}
	};

	struct Card_CS2_125 : public MinionCardBase<Card_CS2_125, Taunt> {};

	struct Card_CS2_122e : public Enchantment<Card_CS2_122e, Attack<1>> {};
	struct Card_CS2_122 : public MinionCardBase<Card_CS2_122> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets const& context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally().Minion() // friendly minions
				.Exclude(context.card_ref_) // only apply on other
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_CS2_122() {
			Aura<Card_CS2_122e, EmitWhenAlive, FlowControl::aura::kUpdateWhenMinionChanges>();
		}
	};

	struct Card_CS2_196 : public MinionCardBase<Card_CS2_196> {
		static void Battlecry(Contexts::OnPlay const& context) {
			SummonToRight(context, Cards::ID_CS2_boar);
		}
	};

	struct Card_EX1_019e : public Enchantment<Card_EX1_019e, MaxHP<1>, Attack<1>> {};
	struct Card_EX1_019 : public MinionCardBase<Card_EX1_019> {
		static bool GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetOptionalBattlecryTargets(context.player_).Ally().Minion().Exclude(context.card_ref_);
			return true;
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			Manipulate(context).OnBoardMinion(target).Enchant().Add<Card_EX1_019e>();
		}
	};

	struct Card_CS2_127 : public MinionCardBase<Card_CS2_127, Taunt> {};
	struct Card_CS2_124 : public MinionCardBase<Card_CS2_124, Charge> {};
	struct Card_EX1_025 : public MinionCardBase<Card_EX1_025> {
		static void Battlecry(Contexts::OnPlay const& context) {
			SummonToRight(context, Cards::ID_EX1_025t);
		}
	};

	struct Card_CS2_147 : public MinionCardBase<Card_CS2_147> {
		static void Battlecry(Contexts::OnPlay const& context) {
			context.manipulate_.Player(context.player_).DrawCard();
		}
	};

	struct Card_CS2_197 : public MinionCardBase<Card_CS2_197, SpellDamage<1>> {};
	struct Card_CS2_179 : public MinionCardBase<Card_CS2_179, Taunt> {};
	struct Card_CS2_131 : public MinionCardBase<Card_CS2_131, Charge> {};
	struct Card_CS2_187 : public MinionCardBase<Card_CS2_187, Taunt> {};
	struct Card_DS1_055 : public MinionCardBase<Card_DS1_055> {
		static void Battlecry(Contexts::OnPlay const& context) {
			ForEach(context, Targets(context.player_).Ally().Exclude(context.card_ref_),
				[context](FlowControl::Manipulate manipulate, state::CardRef ref) {
				manipulate.OnBoardCharacter(ref).Heal(context.card_ref_, 2);
			});
		}
	};

	template <int Arg1>
	struct Card_CS2_226e : public Enchantment<Card_CS2_226e<Arg1>, MaxHP<Arg1>,Attack<Arg1>> {};
	struct Card_CS2_226 : public MinionCardBase<Card_CS2_226> {
		static void Battlecry(Contexts::OnPlay const& context) {
			int count = 0;
			Targets(context.player_).Ally().Minion().Exclude(context.card_ref_).GetInfo()
				.Count(context.manipulate_, &count);

			switch (count) {
			case 0:
				break;
			case 1:
				Manipulate(context).OnBoardMinion(context.card_ref_).Enchant().Add<Card_CS2_226e<1>>();
				break;
			case 2:
				Manipulate(context).OnBoardMinion(context.card_ref_).Enchant().Add<Card_CS2_226e<2>>();
				break;
			case 3:
				Manipulate(context).OnBoardMinion(context.card_ref_).Enchant().Add<Card_CS2_226e<3>>();
				break;
			case 4:
				Manipulate(context).OnBoardMinion(context.card_ref_).Enchant().Add<Card_CS2_226e<4>>();
				break;
			case 5:
				Manipulate(context).OnBoardMinion(context.card_ref_).Enchant().Add<Card_CS2_226e<5>>();
				break;
			case 6:
				Manipulate(context).OnBoardMinion(context.card_ref_).Enchant().Add<Card_CS2_226e<6>>();
				break;
			default:
				throw std::runtime_error("invalid minion count");
			}
		}
	};

	struct Card_EX1_399e : public Enchantment<Card_EX1_399e, Attack<3>> {};
	struct Card_EX1_399 : public MinionCardBase<Card_EX1_399> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::CategorizedOnTakeDamage::Context const& context) {
			Manipulate(context).Card(self).Enchant().Add<Card_EX1_399e>();
			return true;
		};

		Card_EX1_399() {
			RegisterEvent<OnSelfTakeDamage>();
		}
	};

	struct Card_EX1_593 : public MinionCardBase<Card_EX1_593> {
		static void Battlecry(Contexts::OnPlay const& context) {
			context.manipulate_.Hero(context.player_.Opposite()).Damage(context.card_ref_, 3);
		}
	};

	struct Card_CS2_150 : public MinionCardBase<Card_CS2_150> {
		static bool GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetOptionalBattlecryTargets(context.player_);
			return true;
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 2);
		}
	};

	struct Card_CS2_155 : public MinionCardBase<Card_CS2_155, SpellDamage<1>> {};
	struct Card_CS2_162 : public MinionCardBase<Card_CS2_162, Taunt> {};
	struct Card_CS2_213 : public MinionCardBase<Card_CS2_213, Charge> {};

	struct Card_CS2_222o : public Enchantment<Card_CS2_222o, Attack<1>, MaxHP<1>> {};
	struct Card_CS2_222 : public MinionCardBase<Card_CS2_222> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets const& context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally().Minion() // friendly minions
				.Exclude(context.card_ref_) // only apply on other
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_CS2_222() {
			Aura<Card_CS2_222o, EmitWhenAlive, FlowControl::aura::kUpdateWhenMinionChanges>();
		}
	};
}

REGISTER_CARD(GAME_005)
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
REGISTER_CARD(EX1_015)
REGISTER_CARD(EX1_582)
REGISTER_CARD(CS2_141)
REGISTER_CARD(CS2_125)
REGISTER_CARD(CS2_122)
REGISTER_CARD(CS2_196)
REGISTER_CARD(EX1_019)
REGISTER_CARD(CS2_127)
REGISTER_CARD(CS2_124)
