#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=8&sort=-cost&display=1
// Done.

namespace Cards
{
	struct Card_EX1_610 : public SecretCardBase<Card_EX1_610> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::BeforeAttack::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::CardRef hero_ref = context.manipulate_.Board().Player(player).GetHeroRef();
			if (context.defender_ != hero_ref) return true;

			int spell_damage = context.manipulate_.Board().GetSpellDamage(player);
			
			auto op = ([&](state::CardRef card_ref) {
				context.manipulate_.OnBoardCharacter(card_ref).Damage(self, 2 + spell_damage);
			});
			op(context.manipulate_.Board().Player(player.Opposite()).GetHeroRef());
			context.manipulate_.Board().Player(player.Opposite()).minions_.ForEach(op);

			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		};
		Card_EX1_610() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::BeforeAttack>();
		}
	};

	struct Card_EX1_544 : public SpellCardBase<Card_EX1_544> {
		Card_EX1_544() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Board().Player(context.player_.Opposite()).secrets_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardSecret(card_ref).Remove();
				});
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_EX1_611e : public Enchantment<Card_EX1_611e, Cost<2>> {};
	struct Card_EX1_611 : public SecretCardBase<Card_EX1_611> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::BeforeAttack::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::Cards::Card const& attacker_card = context.manipulate_.GetCard(context.attacker_);
			state::PlayerIdentifier attacker_player = attacker_card.GetPlayerIdentifier();
			if (player == attacker_player) return true;

			if (attacker_card.GetZone() != state::kCardZonePlay) return true;
			if (attacker_card.GetHP() <= 0) return true;
			if (attacker_card.GetPendingDestroy()) return true;

			context.manipulate_.OnBoardMinion(context.attacker_).MoveTo<state::kCardZoneHand>();
			context.manipulate_.Card(context.attacker_).Enchant().Add<Card_EX1_611e>();
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		};
		Card_EX1_611() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::BeforeAttack>();
		}
	};

	struct Card_EX1_533 : public SecretCardBase<Card_EX1_533> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::PrepareAttackTarget::Context context) {
			if (!context.defender_->IsValid()) return true;

			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();

			state::Cards::Card const& attacker_card = context.manipulate_.GetCard(context.attacker_);
			state::PlayerIdentifier attacker_player = attacker_card.GetPlayerIdentifier();

			if (player == attacker_player) return true;

			state::CardRef hero_ref = context.manipulate_.Board().Player(player).GetHeroRef();
			if (*context.defender_ != hero_ref) return true;

			state::CardRef new_defender = context.manipulate_.GetRandomTarget(
				TargetsGenerator(player).Alive().Exclude(hero_ref).GetInfo()
			);
			if (!new_defender.IsValid()) return true;

			*context.defender_ = new_defender;
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_533() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::PrepareAttackTarget>();
		}
	};

	struct Card_EX1_554 : public SecretCardBase<Card_EX1_554> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::BeforeAttack::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.GetCard(context.defender_).GetPlayerIdentifier() == player) return true;
			if (context.manipulate_.GetCard(context.defender_).GetCardType() != state::kCardTypeMinion) return true;
			SummonToRightmost(context.manipulate_, player, Cards::ID_EX1_554t);
			SummonToRightmost(context.manipulate_, player, Cards::ID_EX1_554t);
			SummonToRightmost(context.manipulate_, player, Cards::ID_EX1_554t);
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_554() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::BeforeAttack>();
		}
	};

	struct Card_EX1_609 : public SecretCardBase<Card_EX1_609> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionPlayed::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier() == player) return true;
			int spell_damage = context.manipulate_.Board().GetSpellDamage(player);
			context.manipulate_.OnBoardMinion(context.card_ref_).Damage(self, 4 + spell_damage);
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_609() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterMinionPlayed>();
		}
	};

	struct Card_EX1_531e : public Enchantment<Card_EX1_531e, Attack<2>, MaxHP<1>> {};
	struct Card_EX1_531 : public MinionCardBase<Card_EX1_531> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionDied::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (player != context.died_minion_owner_) return true;
			if (context.manipulate_.GetCard(context.card_ref_).GetRace() != state::kCardRaceBeast) return true;
			context.manipulate_.OnBoardMinion(self).Enchant().Add<Card_EX1_531e>();
			return true;
		}
		Card_EX1_531() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterMinionDied>();
		}
	};

	struct Card_EX1_536e : public Enchantment<Card_EX1_536e, MaxHP<1>> {};
	struct Card_EX1_536 : public WeaponCardBase<Card_EX1_536> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::BeforeSecretReveal::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier() != owner) return true;
			context.manipulate_.Weapon(self).Enchant().Add<Card_EX1_536e>();
			return true;
		}
		Card_EX1_536() {
			RegisterEvent<WeaponInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::BeforeSecretReveal>();
		}
	};

	struct Card_EX1_617 : public SpellCardBase<Card_EX1_617> {
		Card_EX1_617() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				if (context.manipulate_.Board().Player(context.player_.Opposite()).minions_.Size() == 0) return false;
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.manipulate_.GetRandomTarget(
					TargetsGenerator(context.player_).Enemy().Minion().GetInfo()
				);
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardMinion(target).Destroy();
			});
		}
	};

	struct Card_EX1_538t : public MinionCardBase<Card_EX1_538t, Charge> {};
	struct Card_EX1_538 : public SpellCardBase<Card_EX1_538> {
		Card_EX1_538() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				if (context.manipulate_.Board().Player(context.player_.Opposite()).minions_.Size() == 0) return false;
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				size_t count = context.manipulate_.Board().Player(context.player_.Opposite()).minions_.Size();
				for (size_t i = 0; i < count; ++i) {
					SummonToRightmost(context.manipulate_, context.player_, Cards::ID_EX1_538t);
				}
			});
		}
	};

	struct Card_EX1_537 : public SpellCardBase<Card_EX1_537> {
		Card_EX1_537() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target_ref = context.GetTarget();
				auto const& target_card = context.manipulate_.GetCard(target_ref);
				if (target_card.GetZone() != state::kCardZonePlay) return;
				
				state::PlayerIdentifier player = target_card.GetPlayerIdentifier();
				int zone_pos = target_card.GetZonePosition();

				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				context.manipulate_.OnBoardMinion(target_ref).Damage(context.card_ref_, 5 + spell_damage);
				auto op = [&](state::PlayerIdentifier player, int zone_pos) {
					if (zone_pos < 0) return;
					auto const& minions = context.manipulate_.Board().Player(player).minions_;
					if (zone_pos >= minions.Size()) return;
					state::CardRef minion_ref = minions.Get(zone_pos);
					context.manipulate_.OnBoardMinion(minion_ref).Damage(context.card_ref_, 2 + spell_damage);
				};
				op(player, zone_pos - 1);
				op(player, zone_pos + 1);
			});
		}
	};

	struct Card_EX1_534 : MinionCardBase<Card_EX1_534> {
		Card_EX1_534() {
			this->deathrattle_handler.Add([](FlowControl::deathrattle::context::Deathrattle context) {
				SummonAt(context, context.player_, context.zone_pos_, Cards::ID_EX1_534t);
				SummonAt(context, context.player_, context.zone_pos_, Cards::ID_EX1_534t);
			});
		}
	};

	struct Card_DS1_188e : public EventHookedEnchantment<Card_DS1_188e, Immune> {
		using EventType = state::Events::EventTypes::AfterAttack;
		static void HandleEvent(EventHookedEnchantmentHandler<Card_DS1_188e> & handler) {
			handler.RemoveEnchantment();
		}
	};
	struct Card_DS1_188 : public WeaponCardBase<Card_DS1_188> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::PrepareAttackTarget::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.attacker_ != context.manipulate_.Board().Player(owner).GetHeroRef()) return true;
			context.manipulate_.Hero(owner).Enchant().AddEventHooked(Card_DS1_188e());
			return true;
		}
		Card_DS1_188() {
			RegisterEvent<WeaponInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::PrepareAttackTarget>();
		}
	};

	struct Card_EX1_543 : public MinionCardBase<Card_EX1_543, Charge> {};
}

REGISTER_CARD(EX1_543)
REGISTER_CARD(DS1_188)
REGISTER_CARD(EX1_534)
REGISTER_CARD(EX1_537)
REGISTER_CARD(EX1_538t)
REGISTER_CARD(EX1_538)
REGISTER_CARD(EX1_617)
REGISTER_CARD(EX1_536)
REGISTER_CARD(EX1_531)
REGISTER_CARD(EX1_609)
REGISTER_CARD(EX1_554)
REGISTER_CARD(EX1_533)
REGISTER_CARD(EX1_611)
REGISTER_CARD(EX1_544)
REGISTER_CARD(EX1_610)
