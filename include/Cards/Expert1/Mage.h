#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=16&sort=-cost&display=1
// Done.

namespace Cards
{
	struct Card_NEW1_012o : public Enchantment<Card_NEW1_012o, Attack<1>> {};
	struct Card_NEW1_012 : public MinionCardBase<Card_NEW1_012> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterSpellPlayed::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;
			context.manipulate_.OnBoardMinion(self).Enchant().Add<Card_NEW1_012o>();
			return true;
		}
		Card_NEW1_012() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::AfterSpellPlayed>();
		}
	};

	struct Card_EX1_608 : public MinionCardBase<Card_EX1_608> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::GetPlayCardCost::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			auto const& card = context.manipulate_.GetCard(context.card_ref_);
			if (card.GetPlayerIdentifier() != owner) return true;
			if (!card.IsSpellCard()) return true;
			*context.cost_ -= 1;
			return true;
		}
		Card_EX1_608() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::GetPlayCardCost>();
		}
	};

	struct Card_EX1_287 : public SecretCardBase<Card_EX1_287> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::CheckPlayCardCountered::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.Board().GetCurrentPlayerId() != player.Opposite()) return true;
			if (!context.manipulate_.GetCard(context.card_ref_).IsSpellCard()) return true;
			*context.countered_ = true;
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		};
		Card_EX1_287() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::CheckPlayCardCountered>();
		}
	};

	struct Card_EX1_289 : public SecretCardBase<Card_EX1_289> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::BeforeAttack::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::CardRef hero_ref = context.manipulate_.Board().Player(player).GetHeroRef();
			if (context.defender_ != hero_ref) return true;
			context.manipulate_.Hero(hero_ref).GainArmor(8);
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_289() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::BeforeAttack>();
		}
	};

	struct Card_EX1_295o : public EnchantmentForThisTurn<Card_EX1_295o, Immune> {};
	struct Card_EX1_295 : public SecretCardBase<Card_EX1_295> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTakeDamage::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::CardRef hero_ref = context.manipulate_.Board().Player(player).GetHeroRef();
			if (context.card_ref_ != hero_ref) return true;
			auto const& hero_card = context.manipulate_.GetCard(hero_ref);
			int hp = hero_card.GetHP() + hero_card.GetArmor();
			if (*context.damage_ < hp) return true;
			*context.damage_ = 0;
			context.manipulate_.Hero(hero_ref).Enchant().Add<Card_EX1_295o>();
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_EX1_295() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::OnTakeDamage>();
		}
	};

	struct Card_EX1_294 : public SecretCardBase<Card_EX1_294> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionPlayed::Context context) {
			state::Cards::Card const& self_card = context.manipulate_.Board().GetCard(self);

			if (context.manipulate_.Board().GetCurrentPlayerId() == self_card.GetPlayerIdentifier()) return true;

			SummonToPlayerByCopy(context, self_card.GetPlayerIdentifier(),
				context.manipulate_.Board().GetCard(context.card_ref_));
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		};

		Card_EX1_294() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::AfterMinionPlayed>();
		}
	};

	struct Card_tt_010 : public SecretCardBase<Card_tt_010> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::PreparePlayCardTarget::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (context.manipulate_.Board().GetCurrentPlayerId() == player) return true;
			if (!context.manipulate_.GetCard(context.card_ref_).IsSpellCard()) return true;

			if (!context.target_ref_->IsValid()) return true;
			if (context.manipulate_.GetCard(*context.target_ref_).GetCardType() != state::kCardTypeMinion) return true;

			state::CardRef new_target = SummonToRightmost(context.manipulate_, player, Cards::ID_tt_010a);
			if (!new_target.IsValid()) return true;

			*context.target_ref_ = new_target;
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		}
		Card_tt_010() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::PreparePlayCardTarget>();
		}
	};

	struct Card_EX1_594 : public SecretCardBase<Card_EX1_594> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::BeforeAttack::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::CardRef hero_ref = context.manipulate_.Board().Player(player).GetHeroRef();
			if (context.defender_ != hero_ref) return true;
			if (context.manipulate_.GetCard(context.attacker_).GetCardType() != state::kCardTypeMinion) return true;

			context.manipulate_.OnBoardMinion(context.attacker_).Destroy();
			context.manipulate_.OnBoardSecret(self).Reveal();
			return false;
		};
		Card_EX1_594() {
			RegisterEvent<SecretInPlayZone, NonCategorized_SelfInLambdaCapture, state::Events::EventTypes::BeforeAttack>();
		}
	};

	struct Card_EX1_612 : public MinionCardBase<Card_EX1_612> {
		static void Battlecry(Contexts::OnPlay const& context) {
			state::PlayerIdentifier player = context.player_;
			int turn = context.manipulate_.Board().GetTurn();
			context.manipulate_.AddEvent<state::Events::EventTypes::GetPlayCardCost>(
				[turn, player](state::Events::EventTypes::GetPlayCardCost::Context context)
			{
				if (context.manipulate_.Board().GetCurrentPlayerId() != player) return true;

				int turn_now = context.manipulate_.Board().GetTurn();
				if (turn_now > turn) return false;
				if (turn_now < turn) return true;

				if (context.manipulate_.GetCard(context.card_ref_).IsSpellCard()) return true;
				*context.cost_ = 0;
				return false;
			});
		}
	};

	struct Card_EX1_275 : public SpellCardBase<Card_EX1_275> {
		Card_EX1_275() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				ApplyToAdjacent(context.manipulate_, context.GetTarget(), [&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Freeze(true);
				});

				ApplyToAdjacent(context.manipulate_, context.GetTarget(), [&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Damage(context.card_ref_, 1);
				});
			});
		}
	};

	struct Card_EX1_274e : public Enchantment<Card_EX1_274e, Attack<2>, MaxHP<2>> {};
	struct Card_EX1_274 : public MinionCardBase<Card_EX1_274> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnEnd::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;
			if (context.manipulate_.Board().Player(owner).secrets_.Empty()) return true;

			context.manipulate_.OnBoardMinion(self).Enchant().Add<Card_EX1_274e>();
			return true;
		}
		Card_EX1_274() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnEnd>();
		}
	};

	struct Card_CS2_028 : public SpellCardBase<Card_CS2_028> {
		Card_CS2_028() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Board().Player(context.player_.Opposite()).minions_.ForEach([&](state::CardRef minion) {
					context.manipulate_.OnBoardMinion(minion).Damage(context.card_ref_, 2);
					context.manipulate_.OnBoardMinion(minion).Freeze(true);
				});
			});
		}
	};

	struct Card_EX1_559 : public MinionCardBase<Card_EX1_559> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnPlay::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;

			if (context.manipulate_.GetCard(context.card_ref_).IsSpellCard()) {
				context.manipulate_.Hero(owner).AddHandCard(Cards::ID_CS2_029);
			}
			return true;
		}
		Card_EX1_559() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnPlay>();
		}
	};

	struct Card_EX1_279 : SpellCardBase<Card_EX1_279> {
		Card_EX1_279() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 10);
			});
		}
	};
}

REGISTER_CARD(EX1_279)
REGISTER_CARD(EX1_559)
REGISTER_CARD(CS2_028)
REGISTER_CARD(EX1_274)
REGISTER_CARD(EX1_275)
REGISTER_CARD(EX1_612)
REGISTER_CARD(EX1_594)
REGISTER_CARD(tt_010)
REGISTER_CARD(EX1_294)
REGISTER_CARD(EX1_295)
REGISTER_CARD(EX1_289)
REGISTER_CARD(EX1_287)
REGISTER_CARD(EX1_608)
REGISTER_CARD(NEW1_012)
