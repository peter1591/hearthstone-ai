#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-premium=1&filter-class=1&sort=-cost&display=1
// finished: Bloodsail Raider

namespace Cards
{
	struct Card_CS2_188_Enchant : public EnchantmentForThisTurn<Card_CS2_188_Enchant, Attack<2>> {};
	struct Card_CS2_188 : public MinionCardBase<Card_CS2_188> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return Targets(context.player_).Minion().Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			if (!context.GetTarget().IsValid()) return;
			Manipulate(context).Card(context.GetTarget()).Enchant().Add<Card_CS2_188_Enchant>();
		}
	};

	struct Card_EX1_009_Enchant : public Enchantment<Card_EX1_009_Enchant, Attack<5>> {};
	struct Card_EX1_009 : public MinionCardBase<Card_EX1_009, Taunt> {
		template <typename Context>
		static auto GetEnrageTargets(Context&& context) {
			context.new_targets.insert(context.card_ref_);
		}
		Card_EX1_009() {
			Enrage<Card_EX1_009_Enchant>();
		}
	};

	struct Card_EX1_008 : public MinionCardBase<Card_EX1_008, Shield> {};

	struct Card_NEW1_025 : public MinionCardBase<Card_NEW1_025> {
		static void Battlecry(Contexts::OnPlay context) {
			state::PlayerIdentifier opponent = 
				context.manipulate_.Board().GetCard(context.card_ref_).GetPlayerIdentifier().Opposite();
			state::CardRef weapon = context.manipulate_.Board().Player(opponent).GetWeaponRef();
			if (!weapon.IsValid()) return;
			Manipulate(context).Weapon(weapon).Damage(context.card_ref_, 1);
		}
	};

	struct Card_NEW1_017e : public Enchantment<Card_NEW1_017e, Attack<2>, MaxHP<2>> {};
	struct Card_NEW1_017 : public MinionCardBase<Card_NEW1_017> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return Targets(context.player_).Minion().Murlocs();
		}
		static void Battlecry(Contexts::OnPlay context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;

			context.manipulate_.Minion(target).Destroy();
			context.manipulate_.Minion(context.card_ref_).Enchant().Add<Card_NEW1_017e>();
		}
	};

	struct Card_EX1_029 : public MinionCardBase<Card_EX1_029> {
		Card_EX1_029() {
			this->deathrattle_handler.Add([](FlowControl::deathrattle::context::Deathrattle context) {
				context.manipulate_.Hero(context.player_.Opposite())
					.Damage(context.card_ref_, 2);
			});
		}
	};

	struct Card_EX1_001e : public Enchantment<Card_EX1_001e, Attack<2>> {};
	struct Card_EX1_001 : public MinionCardBase<Card_EX1_001> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnHeal::Context context) {
			if (context.amount_ < 0) return true;
			if (context.manipulate_.GetCard(context.card_ref_).GetDamage() <= 0) return true;
			context.manipulate_.Minion(self).Enchant().Add<Card_EX1_001e>();
			return true;
		};
		Card_EX1_001() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnHeal>();
		}
	};

	struct Card_EX1_509e : public Enchantment<Card_EX1_509e, Attack<1>> {};
	struct Card_EX1_509 : public MinionCardBase<Card_EX1_509> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionSummoned::Context context) {
			if (context.manipulate_.GetCard(context.card_ref_).GetRace() != state::kCardRaceMurloc) return true;
			if (context.card_ref_ == self) return true;
			context.manipulate_.Minion(self).Enchant().Add<Card_EX1_509e>();
			return true;
		}
		Card_EX1_509() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::AfterMinionSummoned>();
		}
	};

	struct Card_EX1_080o : public Enchantment<Card_EX1_080o, Attack<1>,MaxHP<1>> {};
	struct Card_EX1_080 : public MinionCardBase<Card_EX1_080> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterSecretPlayed::Context context) {
			context.manipulate_.Minion(self).Enchant().Add<Card_EX1_080o>();
			return true;
		}
		Card_EX1_080() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::AfterSecretPlayed>();
		}
	};

	struct Card_EX1_405 : public MinionCardBase<Card_EX1_405, Taunt> {};

	struct Card_CS2_146o : public Enchantment<Card_CS2_146o, Charge> {};
	struct Card_CS2_146 : public MinionCardBase<Card_CS2_146> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::PlayerIdentifier owner_player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			if (!context.manipulate_.Board().Player(owner_player).GetWeaponRef().IsValid()) return;
			context.new_targets.insert(context.card_ref_);
		}
		Card_CS2_146() {
			Aura<Card_CS2_146o, EmitWhenAlive, UpdateAlways>();
		}
	};

	/* charge should be applied as an enchantment
	struct Card_CS2_146 : public MinionCardBase<Card_CS2_146> {
		static void FlagAuraApply(FlowControl::Manipulate & manipulate, state::CardRef target) {
			manipulate.Minion(target).Charge(true);
		}
		static void FlagAuraRemove(FlowControl::Manipulate & manipulate, state::CardRef target) {
		}
		Card_CS2_146() {
			OwnerCardFlagAura<AliveWhenInPlay>();
		}
	};
	*/

	struct Card_EX1_010 : public MinionCardBase<Card_EX1_010, Stealth> {};
	struct Card_CS2_169 : public MinionCardBase<Card_CS2_169, Windfury> {};
	
	struct Card_EX1_004e : public Enchantment<Card_EX1_004e, MaxHP<1>> {};
	struct Card_EX1_004 : public MinionCardBase<Card_EX1_004> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnEnd::Context context) {
			auto player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.Board().GetCurrentPlayerId() != player) return true;

			auto targets = TargetsGenerator(player).Ally(player).Minion().Exclude(self);
			auto target = context.manipulate_.GetRandomTarget(targets.GetInfo());

			if (target.IsValid()) Manipulate(context).Card(self).Enchant().Add<Card_EX1_004e>();
			return true;
		};
		Card_EX1_004() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnEnd>();
		}
	};

	struct Card_EX1_393_Enchant : public Enchantment<Card_EX1_393_Enchant, Attack<3>> {};
	struct Card_EX1_393 : public MinionCardBase<Card_EX1_393> {
		template <typename Context>
		static auto GetEnrageTargets(Context&& context) {
			context.new_targets.insert(context.card_ref_);
		}
		Card_EX1_393() {
			Enrage<Card_EX1_393_Enchant>();
		}
	};

	struct Card_EX1_045 : public MinionCardBase<Card_EX1_045, CantAttack> {};
	struct Card_EX1_012 : public MinionCardBase<Card_EX1_012, SpellDamage<1>> {
		Card_EX1_012() {
			deathrattle_handler.Add([](FlowControl::deathrattle::context::Deathrattle context) {
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};
	
	template <int v>
	struct Card_NEW1_018e : public Enchantment<Card_NEW1_018e<v>, Attack<v>> {};
	struct Card_NEW1_018 : public MinionCardBase<Card_NEW1_018> {
		static void Battlecry(Contexts::OnPlay context) {
			state::CardRef weapon_ref = context.manipulate_.Board().Player(context.player_).GetWeaponRef();
			if (!weapon_ref.IsValid()) return;
			int weapon_atk = context.manipulate_.GetCard(weapon_ref).GetAttack();
			if (weapon_atk <= 0) return;
			context.manipulate_.Minion(context.card_ref_).Enchant().Add<Card_NEW1_018e>(weapon_atk);
		}
	};

	template <int v>
	struct Card_EX1_059_atk : public Enchantment<Card_EX1_059_atk<v>, SetAttack<v>> {};
	template <int v>
	struct Card_EX1_059_hp : public Enchantment<Card_EX1_059_hp<v>, SetMaxHP<v>> {};
	struct Card_EX1_059 : public MinionCardBase<Card_EX1_059> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return Targets(context.player_).Minion().Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			state::Cards::Card const& card = context.manipulate_.GetCard(target);
			int hp = card.GetHP();
			int atk = card.GetAttack();
			context.manipulate_.Minion(target).Enchant().Add<Card_EX1_059_atk>(hp);
			context.manipulate_.Minion(target).Enchant().Add<Card_EX1_059_hp>(atk);
			context.manipulate_.Minion(target).Enchant().SetHealthToMaxHP();
		}
	};

	struct Card_EX1_162o : public Enchantment<Card_EX1_162o, Attack<1>> {};
	struct Card_EX1_162 : public MinionCardBase<Card_EX1_162> {
		Card_EX1_162() {
			AdjacentBuffAura<Card_EX1_162o>();
		}
	};


	struct Card_EX1_089 : public MinionCardBase<Card_EX1_089> {
		static void Battlecry(Contexts::OnPlay context) {
			AnotherPlayer(context).GetResource().GainEmptyCrystal();
		}
	};

	struct Card_NEW1_038_Enchant : public Enchantment<Card_NEW1_038_Enchant, Attack<1>, MaxHP<1>> {};
	struct Card_NEW1_038 : public MinionCardBase<Card_NEW1_038> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnEnd::Context context) {
			Manipulate(context).Card(self).Enchant().Add<Card_NEW1_038_Enchant>();
			return true;
		};
		Card_NEW1_038() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnEnd>();
		}
	};

	struct Card_EX1_020 : public MinionCardBase<Card_EX1_020, Shield> {};
	struct Card_CS1_069 : public MinionCardBase<Card_CS1_069, Taunt> {};

	struct Card_CS2_203 : public MinionCardBase<Card_CS2_203> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return Targets(context.player_).Minion().Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			if (!context.GetTarget().IsValid()) return;
			return Manipulate(context).Minion(context.GetTarget()).Silence();
		}
	};

	struct Card_EX1_390_Enchant : public Enchantment<Card_EX1_390_Enchant, Attack<3>> {};
	struct Card_EX1_390 : public MinionCardBase<Card_EX1_390, Taunt> {
		template <typename Context>
		static auto GetEnrageTargets(Context&& context) {
			context.new_targets.insert(context.card_ref_);
		}
		Card_EX1_390() {
			Enrage<Card_EX1_390_Enchant>();
		}
	};

	struct Card_EX1_564 : public MinionCardBase<Card_EX1_564> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return Targets(context.player_).Minion().Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			if (!context.GetTarget().IsValid()) return;

			state::CardRef new_ref = Manipulate(context).Minion(context.card_ref_)
				.BecomeCopyof(context.GetTarget());

			*context.new_card_ref = new_ref;
		}
	};
}

REGISTER_CARD(EX1_162)
REGISTER_CARD(EX1_059)
REGISTER_CARD(NEW1_018)
REGISTER_CARD(EX1_012)
REGISTER_CARD(EX1_045)
REGISTER_CARD(EX1_393)
REGISTER_CARD(EX1_004)
REGISTER_CARD(CS2_169)
REGISTER_CARD(EX1_010)
REGISTER_CARD(CS2_146)
REGISTER_CARD(EX1_405)
REGISTER_CARD(EX1_080)
REGISTER_CARD(EX1_509)
REGISTER_CARD(EX1_001)
REGISTER_CARD(EX1_029)
REGISTER_CARD(NEW1_017)
REGISTER_CARD(NEW1_025)
REGISTER_CARD(EX1_564)
REGISTER_CARD(EX1_008)
REGISTER_CARD(EX1_009)
REGISTER_CARD(EX1_390)
REGISTER_CARD(CS2_188)
REGISTER_CARD(CS2_203)

REGISTER_CARD(EX1_089)
REGISTER_CARD(NEW1_038)
REGISTER_CARD(EX1_020)
REGISTER_CARD(CS1_069)
