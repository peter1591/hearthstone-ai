#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=32&sort=-cost&display=1
// Done.

namespace Cards
{
	struct Card_EX1_363e : public EventHookedEnchantment<Card_EX1_363e> {
		using EventType = state::Events::EventTypes::BeforeAttack;
		static void HandleEvent(EventHookedEnchantmentHandler<Card_EX1_363e> & handler) {
			if (handler.context.attacker_ != handler.card_ref) return;
			handler.context.manipulate_.Hero(handler.aux_data.player).DrawCard();
		}
	};
	struct Card_EX1_363 : public SpellCardBase<Card_EX1_363> {
		Card_EX1_363() {
			onplay_handler.SetSpecifyTargetCallback([](FlowControl::onplay::context::GetSpecifiedTarget & context) {
				context.SetRequiredTargets(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().AddEventHooked(Card_EX1_363e());
			});
		}
	};

	struct Card_EX1_132 : public SecretCardBase<Card_EX1_132> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterTakenDamage::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.card_ref_ != context.manipulate_.Board().Player(player).GetHeroRef()) return true;
			context.manipulate_.Hero(player.Opposite()).Damage(self, context.damage_);
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_132() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterTakenDamage>();
		}
	};

	struct Card_EX1_130 : public SecretCardBase<Card_EX1_130> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::PrepareAttackTarget::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.GetCard(context.attacker_).GetPlayerIdentifier() == player) return true;
			state::CardRef new_target = SummonToRightmost(context.manipulate_, player, Cards::ID_EX1_130a);
			if (!new_target.IsValid()) return true;
			*context.defender_ = new_target;
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_130() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::PrepareAttackTarget>();
		}
	};

	struct Card_EX1_136e : public Enchantment<Card_EX1_136e, SetMaxHP<1>> {};
	struct Card_EX1_136 : public SecretCardBase<Card_EX1_136> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionDied::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.Board().GetCurrentPlayerId() == player) return true;
			if (context.died_minion_owner_ != player) return true;

			Cards::CardId card_id = context.manipulate_.GetCard(context.card_ref_).GetCardId();
			state::CardRef new_minion = SummonToRightmost(context.manipulate_, player, card_id);
			if (!new_minion.IsValid()) return true;

			context.manipulate_.OnBoardMinion(new_minion).Enchant().Add<Card_EX1_136e>();
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_136() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterMinionDied>();
		}
	};

	struct Card_EX1_379e : public Enchantment<Card_EX1_379e, SetMaxHP<1>> {};
	struct Card_EX1_379 : public SecretCardBase<Card_EX1_379> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionPlayed::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.Board().GetCurrentPlayerId() == player) return true;
			context.manipulate_.OnBoardMinion(context.card_ref_).Enchant().Add<Card_EX1_379e>();
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_379() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterMinionPlayed>();
		}
	};

	struct Card_EX1_619e : public Enchantment<Card_EX1_619e, MaxHP<1>> {};
	struct Card_EX1_619 : public SpellCardBase<Card_EX1_619> {
		Card_EX1_619() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				auto op = [&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Enchant().Add<Card_EX1_619e>();
				};
				context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
				context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
			});
		}
	};

	struct Card_EX1_362 : public MinionCardBase<Card_EX1_362> {
		static bool GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetOptionalTargets(context.player_).Ally().Targetable();
			return true;
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.OnBoardMinion(target).Shield(true);
		}
	};

	struct Card_EX1_366e : public Enchantment<Card_EX1_366e, Attack<1>, MaxHP<1>> {};
	struct Card_EX1_366 : public WeaponCardBase<Card_EX1_366> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionSummoned::Context context) {
			if (context.manipulate_.GetCard(self).GetHP() < 0) return false;
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::PlayerIdentifier minion_owner = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			if (owner != minion_owner) return true;
			context.manipulate_.OnBoardMinion(context.card_ref_).Enchant().Add<Card_EX1_366e>();
			context.manipulate_.Weapon(self).Damage(self, 1);
			return true;
		}
		Card_EX1_366() {
			RegisterEvent<WeaponInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::AfterMinionSummoned>();
		}
	};

	struct Card_EX1_349 : public SpellCardBase<Card_EX1_349> {
		Card_EX1_349() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int owner_hand = (int)context.manipulate_.Board().Player(context.player_).hand_.Size();
				int opponent_hand = (int)context.manipulate_.Board().Player(context.player_.Opposite()).hand_.Size();
				if (opponent_hand >= owner_hand) return;
				int draws = owner_hand - opponent_hand;
				for (int i = 0; i < draws; ++i) {
					context.manipulate_.Hero(context.player_).DrawCard();
				}
			});
		}
	};

	struct Card_EX1_382e : public Enchantment<Card_EX1_382e, SetAttack<1>> {};
	struct Card_EX1_382 : public MinionCardBase<Card_EX1_382> {
		static bool GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetOptionalTargets(context.player_).Ally().Targetable();
			return true;
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_EX1_382e>();
		}
	};

	template <int v>
	struct Card_EX1_355e : public Enchantment<Card_EX1_355e<v>, Attack<v>> {};
	struct Card_EX1_355 : public SpellCardBase<Card_EX1_355> {
		Card_EX1_355() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				int attack = context.manipulate_.GetCard(target).GetAttack();
				context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_EX1_355e>(attack);
			});
		}
	};

	struct Card_EX1_365 : public SpellCardBase<Card_EX1_365> {
		Card_EX1_365() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				Cards::CardId drawn_card_id = (Cards::CardId)(-1);
				context.manipulate_.Hero(context.player_).DrawCard(&drawn_card_id);
				if (drawn_card_id < 0) return;
				state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(drawn_card_id);
				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, raw_card.enchanted_states.cost);
			});
		}
	};

	struct Card_EX1_384 : SpellCardBase<Card_EX1_384> {
		Card_EX1_384() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int damage = 0;
				context.manipulate_
					.Board()
					.CalculateFinalDamageAmount(context.card_ref_, 8, &damage);

				Targets targets = TargetsGenerator(context.player_).Enemy().Alive().GetInfo();
				for (int i = 0; i < damage; ++i) {
					state::CardRef target = context.manipulate_.GetRandomTarget(targets);
					assert(target.IsValid());
					context.manipulate_
						.OnBoardCharacter(target)
						.ConductFinalDamage(context.card_ref_, 1);
				}
			});
		}
	};

	struct Card_EX1_354 : public SpellCardBase<Card_EX1_354> {
		Card_EX1_354() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Heal(context.card_ref_, 8);
				context.manipulate_.Hero(context.player_).DrawCard();
				context.manipulate_.Hero(context.player_).DrawCard();
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_EX1_383 : public MinionCardBase<Card_EX1_383, Taunt, Shield> {
		Card_EX1_383() {
			this->deathrattle_handler.Add([](FlowControl::deathrattle::context::Deathrattle context) {
				context.manipulate_.Hero(context.player_).EquipWeapon(Cards::ID_EX1_383t);
			});
		}
	};
}

REGISTER_CARD(EX1_383)
REGISTER_CARD(EX1_354)
REGISTER_CARD(EX1_384)
REGISTER_CARD(EX1_365)
REGISTER_CARD(EX1_355)
REGISTER_CARD(EX1_382)
REGISTER_CARD(EX1_349)
REGISTER_CARD(EX1_366)
REGISTER_CARD(EX1_362)
REGISTER_CARD(EX1_619)
REGISTER_CARD(EX1_379)
REGISTER_CARD(EX1_136)
REGISTER_CARD(EX1_130)
REGISTER_CARD(EX1_132)
REGISTER_CARD(EX1_363)
