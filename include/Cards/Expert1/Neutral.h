#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-premium=1&filter-class=1&sort=-cost&display=1
// finished: Pint-Sized Summoner

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
		static auto GetEnrageTarget(Context&& context) {
			context.new_target = context.card_ref_;
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
		static auto GetAuraTarget(FlowControl::aura::contexts::AuraGetTarget context) {
			state::PlayerIdentifier owner_player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			if (!context.manipulate_.Board().Player(owner_player).GetWeaponRef().IsValid()) return;
			context.new_target = context.card_ref_;
		}
		Card_CS2_146() {
			SingleEnchantmentAura<Card_CS2_146o, EmitWhenAlive, FlowControl::aura::kUpdateAlways>();
		}
	};

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
		static auto GetEnrageTarget(Context&& context) {
			context.new_target = context.card_ref_;
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

	struct Card_NEW1_021 : public MinionCardBase<Card_NEW1_021> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnStart::Context context) {
			if (context.manipulate_.GetCard(self).GetPlayerIdentifier() != context.manipulate_.Board().GetCurrentPlayerId()) return true;
			auto op = [&](state::CardRef ref) {
				context.manipulate_.Minion(ref).Destroy();
			};
			context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
			context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
			return true;
		};
		Card_NEW1_021() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnStart>();
		}
	};

	struct Card_NEW1_023 : public MinionCardBase<Card_NEW1_023, ImmuneToSpellAndHeroPower> {};

	struct Card_NEW1_019 : public MinionCardBase<Card_NEW1_019> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionSummoned::Context context) {
			if (self == context.card_ref_) return true;
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::PlayerIdentifier summoning = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			if (owner != summoning) return true;
			
			state::CardRef target = context.manipulate_.GetRandomTarget(TargetsGenerator(owner).Enemy(owner).Alive().GetInfo());
			if (!target.IsValid()) return true;
			context.manipulate_.Character(target).Damage(context.card_ref_, 1);
			return true;
		};
		Card_NEW1_019() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::AfterMinionSummoned>();
		}
	};

	struct Card_EX1_096 : public MinionCardBase<Card_EX1_096> {
		Card_EX1_096() {
			deathrattle_handler.Add([](FlowControl::deathrattle::context::Deathrattle context) {
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_EX1_100 : public MinionCardBase<Card_EX1_100> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterSpellPlayed::Context context) {
			int card_id = context.manipulate_.GetCard(context.card_ref_).GetCardId();
			state::PlayerIdentifier player = context.manipulate_.Board().GetCurrentPlayerId().Opposite();
			context.manipulate_.Hero(player).AddHandCard(card_id);
			return true;
		};
		Card_EX1_100() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::AfterSpellPlayed>();
		}
	};

	struct Card_EX1_082 : public MinionCardBase<Card_EX1_082> {
		static void Battlecry(Contexts::OnPlay context) {
			for (int i = 0; i < 3; ++i) {
				state::CardRef target = context.manipulate_.GetRandomTarget(
					TargetsGenerator(context.player_).Alive().Exclude(context.card_ref_).GetInfo()
				);
				context.manipulate_.Character(target).Damage(context.card_ref_, 1);
			}
		}
	};

	struct Card_EX1_055o : public EnchantmentForThisTurn<Card_EX1_055o, Attack<2>> {};
	struct Card_EX1_055 : public MinionCardBase<Card_EX1_055> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterSpellPlayed::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.player_) return true;
			context.manipulate_.Minion(self).Enchant().Add<Card_EX1_055o>();
			return true;
		};
		Card_EX1_055() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::AfterSpellPlayed>();
		}
	};

	struct Card_EX1_616 : public MinionCardBase<Card_EX1_616> {
		static void AuraApplyOn(FlowControl::aura::contexts::AuraApplyFlagOnBoard context) {
			context.manipulate_.Board().IncreaseMinionCostExtra(1);
		}
		static void AuraRemoveFrom(FlowControl::aura::contexts::AuraRemoveFlagFromBoard context) {
			context.manipulate_.Board().DecreaseMinionCostExtra(1);
		}
		Card_EX1_616() {
			BoardFlagAura<EmitWhenAlive>();
		}
	};

	struct Card_NEW1_037e : public Enchantment<Card_NEW1_037e, Attack<1>> {};
	struct Card_NEW1_037 : public MinionCardBase<Card_NEW1_037> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnEnd::Context context) {
			auto player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.Board().GetCurrentPlayerId() != player) return true;

			auto targets = TargetsGenerator(player).Ally(player).Minion().Exclude(self);
			auto target = context.manipulate_.GetRandomTarget(targets.GetInfo());

			if (target.IsValid()) Manipulate(context).Card(self).Enchant().Add<Card_NEW1_037e>();
			return true;
		};
		Card_NEW1_037() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnEnd>();
		}
	};

	struct Card_NEW1_029 : public MinionCardBase<Card_NEW1_029> {
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Board().Player(context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier())
				.next_spell_cost_zero_this_turn_ = true;
		}
	};

	struct Card_EX1_557 : public MinionCardBase<Card_EX1_557> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnStart::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;

			if (context.manipulate_.GetRandom().Get(2) == 0) return true;
			context.manipulate_.Hero(owner).DrawCard();
			return true;
		}
		Card_EX1_557() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnStart>();
		}
	};

	struct Card_EX1_076 : public MinionCardBase<Card_EX1_076> {
		static void AuraApplyOn(FlowControl::aura::contexts::AuraApplyFlagOnOwnerPlayer context) {
			--context.manipulate_.Board().Player(context.player_).first_minion_each_turn_cost_bias_;
		}
		static void AuraRemoveFrom(FlowControl::aura::contexts::AuraRemoveFlagFromOwnerPlayer context) {
			++context.manipulate_.Board().Player(context.player_).first_minion_each_turn_cost_bias_;
		}
		Card_EX1_076() {
			OwnerPlayerFlagAura<EmitWhenAlive>();
		}
	};

	struct Card_EX1_058 : public MinionCardBase<Card_EX1_058> {
		static void Battlecry(Contexts::OnPlay context) {
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			assert(card.GetCardType() == state::kCardTypeMinion);
			assert(card.GetZone() == state::kCardZonePlay);

			state::PlayerIdentifier player = card.GetPlayerIdentifier();
			auto & minions = context.manipulate_.Board().Player(player).minions_;
			int zone_pos = card.GetZonePosition();

			if (zone_pos > 0) {
				context.manipulate_.Minion(minions.Get(zone_pos - 1)).Taunt(true);
			}
			if (zone_pos < minions.Size() - 1) {
				context.manipulate_.Minion(minions.Get(zone_pos + 1)).Taunt(true);
			}
		}
	};

	struct Card_NEW1_020 : public MinionCardBase<Card_NEW1_020> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterSpellPlayed::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;

			auto op = [&](state::CardRef ref) {
				context.manipulate_.Minion(ref).Damage(self, 1);
			};
			context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
			context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
			return true;
		}
		Card_NEW1_020() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::AfterSpellPlayed>();
		}
	};

	struct Card_EX1_049 : public MinionCardBase<Card_EX1_049> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			return TargetsGenerator(context.player_).Ally(context).Minion();
		}
		static void Battlecry(Contexts::OnPlay context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.Minion(target).MoveTo<state::kCardZonePlay, state::kCardZoneHand>(context.player_);
		}
	};

	struct Card_EX1_007 : public MinionCardBase<Card_EX1_007> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTakeDamage::Context context) {
			context.manipulate_.Hero(
				context.manipulate_.GetCard(self).GetPlayerIdentifier()
			).DrawCard();
			return true;
		};

		Card_EX1_007() {
			RegisterEvent<OnSelfTakeDamage>();
		}
	};

	struct Card_EX1_006 : public MinionCardBase<Card_EX1_006> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnStart::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.Board().GetCurrentPlayerId() != player) return true;

			std::vector<state::CardRef> candidates;
			context.manipulate_.Board().Player(player).hand_.ForEach([&](state::CardRef ref) {
				if (context.manipulate_.GetCard(ref).GetCardType() != state::kCardTypeMinion) return true;
				candidates.push_back(ref);
				return true;
			});

			if (candidates.empty()) return true;

			size_t rand = context.manipulate_.GetRandom().Get(candidates.size());
			state::CardRef target = candidates[rand];

			// swap minion
			context.manipulate_.Minion(self).SwapWith<state::kCardZonePlay, state::kCardZoneHand>(target);
			return true;
		}
		Card_EX1_006() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnStart>();
		}
	};

	struct Card_EX1_089 : public MinionCardBase<Card_EX1_089> {
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Board().Player(context.player_.Opposite()).GetResource().GainCrystal();
		}
	};

	template <int v>
	struct Card_EX1_590e : public Enchantment<Attack<3*v>, MaxHP<3*v>> {};
	struct Card_EX1_590 : public MinionCardBase<Card_EX1_590> {
		static void Battlecry(Contexts::OnPlay context) {
			int count = 0;
			auto op = [&](state::CardRef ref) {
				if (!context.manipulate_.GetCard(ref).HasShield()) return;

				++count;
				context.manipulate_.Minion(ref).Shield(false);
			};

			if (count <= 0) return;
			context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
			context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
			context.manipulate_.Minion(context.card_ref_).Enchant().Add<Card_EX1_590e>(count);
		}
	};

	struct Card_EX1_050 : public MinionCardBase<Card_EX1_050> {
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Hero(state::PlayerIdentifier::First()).DrawCard();
			context.manipulate_.Hero(state::PlayerIdentifier::First()).DrawCard();
			context.manipulate_.Hero(state::PlayerIdentifier::Second()).DrawCard();
			context.manipulate_.Hero(state::PlayerIdentifier::Second()).DrawCard();
		}
	};

	struct Card_EX1_103e : public Enchantment<MaxHP<2>> {};
	struct Card_EX1_103 : public MinionCardBase<Card_EX1_103> {
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Board().Player(context.player_).minions_.ForEach([&](state::CardRef ref) {
				if (ref == context.card_ref_) return;
				if (context.manipulate_.GetCard(ref).GetCardType() != state::kCardRaceMurloc) return;
				context.manipulate_.Minion(ref).Enchant().Add<Card_EX1_103e>();
			});
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
		static auto GetEnrageTarget(Context&& context) {
			context.new_target = context.card_ref_;
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

REGISTER_CARD(EX1_103)
REGISTER_CARD(EX1_050)
REGISTER_CARD(EX1_590)
REGISTER_CARD(EX1_089)
REGISTER_CARD(EX1_006)
REGISTER_CARD(EX1_007)
REGISTER_CARD(EX1_049)
REGISTER_CARD(NEW1_020)
REGISTER_CARD(EX1_058)
REGISTER_CARD(EX1_076)
REGISTER_CARD(EX1_557)
REGISTER_CARD(NEW1_029)
REGISTER_CARD(NEW1_037)
REGISTER_CARD(EX1_616)
REGISTER_CARD(EX1_055)
REGISTER_CARD(EX1_082)
REGISTER_CARD(EX1_100)
REGISTER_CARD(EX1_096)
REGISTER_CARD(NEW1_019)
REGISTER_CARD(NEW1_023)
REGISTER_CARD(NEW1_021)
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

REGISTER_CARD(NEW1_038)
REGISTER_CARD(EX1_020)
REGISTER_CARD(CS1_069)
