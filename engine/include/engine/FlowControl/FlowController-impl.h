#pragma once

#include "engine/FlowControl/FlowController.h"
#include "engine/FlowControl/ValidActionGetter.h"
#include "engine/FlowControl/ActionTargetIndex.h"

#include "engine/FlowControl/Manipulate-impl.h"

namespace engine {
	namespace FlowControl
	{
		inline engine::Result FlowController::PerformAction()
		{
			auto main_op = flow_context_.GetMainOp();

			switch (main_op) {
			case engine::kMainOpPlayCard:
				return PlayCard(flow_context_.ChooseHandCard());
			case engine::kMainOpAttack:
				return Attack(flow_context_.GetAttacker());
			case engine::kMainOpHeroPower:
				return HeroPower();
			case engine::kMainOpEndTurn:
				return EndTurn();
			default:
				return engine::Result::kResultInvalid;
			}
		}

		inline engine::Result FlowController::PlayCard(int hand_idx)
		{
			assert(ValidActionGetter(state_).IsPlayable(hand_idx));

			flow_context_.Reset();

			state::CardRef card_ref = state_.GetCurrentPlayer().hand_.Get(hand_idx);

			PlayCardInternal(card_ref);

			assert(flow_context_.Empty());
			return flow_context_.GetResult();
		}

		inline engine::Result FlowController::EndTurn()
		{
			flow_context_.Reset();
			EndTurnInternal();

			assert(flow_context_.Empty());
			return flow_context_.GetResult();
		}

		inline engine::Result FlowController::Attack(state::CardRef attacker)
		{
			assert(attacker.IsValid());

			flow_context_.Reset();
			AttackInternal(attacker);

			assert(flow_context_.Empty());
			return flow_context_.GetResult();
		}

		inline engine::Result FlowController::HeroPower()
		{
			flow_context_.Reset();
			HeroPowerInternal();

			assert(flow_context_.Empty());
			return flow_context_.GetResult();
		}

		inline engine::Result FlowController::Resolve()
		{
			flow_context_.GetResolver().Resolve(state_, flow_context_);
			return flow_context_.GetResult();
		}

		inline void FlowController::PlayCardInternal(state::CardRef card_ref)
		{
			switch (state_.GetCardsManager().Get(card_ref).GetCardType())
			{
			case state::kCardTypeMinion:
				assert(!state_.GetCurrentPlayer().minions_.Full());
				if (!PlayCardPhase<state::kCardTypeMinion>(card_ref)) return;
				break;
			case state::kCardTypeWeapon:
				if (!PlayCardPhase<state::kCardTypeWeapon>(card_ref)) return;
				break;
			case state::kCardTypeSpell:
				assert([&]() {
					if (!state_.GetCard(card_ref).IsSecretCard()) return true;
					if (state_.GetCurrentPlayer().secrets_.Exists(state_.GetCard(card_ref).GetCardId())) return false;
					return true;
				}());
				if (!PlayCardPhase<state::kCardTypeSpell>(card_ref)) return;
				break;
			default:
				assert(false);
				return;
			}

			if (!flow_context_.GetResolver().Resolve(state_, flow_context_)) return;

			++state_.GetCurrentPlayer().played_cards_this_turn_;
		}

		template <state::CardType CardType>
		inline bool FlowController::PlayCardPhase(state::CardRef card_ref)
		{
			int cost = state_.GetCard(card_ref).GetCost();
			bool cost_health_instead = false;
			state_.TriggerEvent<state::Events::EventTypes::GetPlayCardCost>(state::Events::EventTypes::GetPlayCardCost::Context{
				state_, card_ref, &cost, &cost_health_instead
			});

			if (cost_health_instead) CostHealth(cost);
			else CostCrystal(cost);

			state_.IncreasePlayOrder();
			Manipulate(state_, flow_context_).Card(card_ref).SetPlayOrder();

			state_.GetCard(card_ref).GetRawData().onplay_handler.PrepareTarget(state_, flow_context_, state_.GetCurrentPlayerId(), card_ref);

			state::CardRef onplay_target = flow_context_.GetSpecifiedTarget();
			if (onplay_target.IsValid()) {
				state_.TriggerEvent<state::Events::EventTypes::PreparePlayCardTarget>(state::Events::EventTypes::PreparePlayCardTarget::Context{
					Manipulate(state_, flow_context_), card_ref, &onplay_target
				});
				assert(onplay_target.IsValid());
				flow_context_.ChangeSpecifiedTarget(onplay_target);
			}

			state_.TriggerEvent<state::Events::EventTypes::OnPlay>(
				state::Events::EventTypes::OnPlay::Context{ Manipulate(state_, flow_context_), card_ref });

			bool countered = false;
			state_.TriggerEvent<state::Events::EventTypes::CheckPlayCardCountered>(
				state::Events::EventTypes::CheckPlayCardCountered::Context{ Manipulate(state_, flow_context_), card_ref, &countered });
			if (countered) {
				state_.GetZoneChanger<state::kCardZoneHand>(card_ref)
					.ChangeTo<state::kCardZoneGraveyard>(state_.GetCurrentPlayerId());
				return true;
			}

			state_.GetCurrentPlayer().GetResource().IncreaseNextOverload(state_.GetCard(card_ref).GetRawData().overload);

			return PlayCardPhaseInternal<CardType>(card_ref);
		}

		inline void FlowController::CostCrystal(int amount) {
			if (amount <= 0) return;

			auto& crystal = state_.GetCurrentPlayer().GetResource();
			if (crystal.GetCurrent() < amount) {
				assert(false); // should only pass playable card
			}
			crystal.Cost(amount);
		}

		inline void FlowController::CostHealth(int amount) {
			if (amount <= 0) return;

			state::CardRef hero_ref = state_.GetCurrentPlayer().GetHeroRef();
			if (state_.GetCard(hero_ref).GetHP() < amount) {
				assert(false); // should only pass playable card
			}
			Manipulate(state_, flow_context_).Hero(hero_ref).Damage(hero_ref, amount);
		}

		template <> inline bool FlowController::PlayCardPhaseInternal<state::kCardTypeMinion>(state::CardRef card_ref)
		{
			assert(state_.GetCard(card_ref).GetCardType() == state::kCardTypeMinion);
			return PlayMinionCardPhase(card_ref);
		}

		template <> inline bool FlowController::PlayCardPhaseInternal<state::kCardTypeWeapon>(state::CardRef card_ref)
		{
			assert(state_.GetCard(card_ref).GetCardType() == state::kCardTypeWeapon);
			return PlayWeaponCardPhase(card_ref);
		}

		template <> inline bool FlowController::PlayCardPhaseInternal<state::kCardTypeSpell>(state::CardRef card_ref)
		{
			assert(state_.GetCard(card_ref).GetCardType() == state::kCardTypeSpell);
			return PlaySpellCardPhase(card_ref);
		}

		template <> inline bool FlowController::PlayCardPhaseInternal<state::kCardTypeHeroPower>(state::CardRef card_ref)
		{
			assert(state_.GetCard(card_ref).GetCardType() == state::kCardTypeHeroPower);
			return PlayHeroPowerCardPhase(card_ref);
		}

		inline bool FlowController::PlayMinionCardPhase(state::CardRef card_ref)
		{
			state_.TriggerEvent<state::Events::EventTypes::BeforeMinionSummoned>(
				state::Events::EventTypes::BeforeMinionSummoned::Context{ Manipulate(state_, flow_context_), card_ref });

			if (state_.GetCurrentPlayer().minions_.Full()) {
				// before playing this minion card, the board is not full (we've ensured this in an assertion)
				// but, the board is full before we actually put the minion on the board
				// this might due to some pre-summon triggers
				// we should consider this as an valid action, since the player can actually play the minion card
				return true;
			}

			int total_minions = (int)state_.GetCurrentPlayer().minions_.Size();
			int put_position = flow_context_.GetMinionPutLocation(total_minions);

			state_.GetZoneChanger<state::kCardTypeMinion, state::kCardZoneHand>(card_ref)
				.ChangeTo<state::kCardZonePlay>(state_.GetCurrentPlayerId(), put_position);

			assert(state_.GetCard(card_ref).GetPlayerIdentifier() == state_.GetCurrentPlayerId());
			state::CardRef new_card_ref;
			state_.GetCard(card_ref).GetRawData().onplay_handler.OnPlay(state_, flow_context_, state_.GetCurrentPlayerId(), card_ref, &new_card_ref);
			if (!new_card_ref.IsValid()) {
				// no transformation
				new_card_ref = card_ref;
			}
#ifndef NDEBUG
			[&]() {
				if (card_ref == new_card_ref) return;
				// minion transformed
				if (state_.GetCard(card_ref).GetZone() == state::kCardZonePlay) assert(false);
				if (state_.GetCard(new_card_ref).GetZone() != state::kCardZonePlay) assert(false);
				// might transform to hero type (e.g., Lord Jaraxxus)
			}();
#endif

			++state_.GetCurrentPlayer().played_minions_this_turn_;

			state_.TriggerEvent<state::Events::EventTypes::AfterMinionPlayed>(
				state::Events::EventTypes::AfterMinionPlayed::Context{ Manipulate(state_, flow_context_), new_card_ref });

			state_.TriggerEvent<state::Events::EventTypes::AfterMinionSummoned>(
				state::Events::EventTypes::AfterMinionSummoned::Context{ Manipulate(state_, flow_context_), new_card_ref });

			if (state_.GetCard(new_card_ref).GetCardType() == state::kCardTypeMinion) {
				Manipulate(state_, flow_context_).OnBoardMinion(new_card_ref).AfterSummoned();
			}

			return true;
		}

		inline bool FlowController::PlayWeaponCardPhase(state::CardRef card_ref)
		{
			Manipulate(state_, flow_context_).CurrentPlayer().EquipWeapon<state::kCardZoneHand>(card_ref);

			state::CardRef new_card_ref;

			assert(state_.GetCard(card_ref).GetPlayerIdentifier() == state_.GetCurrentPlayerId());
			state_.GetCard(card_ref).GetRawData().onplay_handler.OnPlay(state_, flow_context_, state_.GetCurrentPlayerId(), card_ref, &new_card_ref);
			assert(!new_card_ref.IsValid()); // weapon cannot transformed

			return true;
		}

		inline bool FlowController::PlayHeroPowerCardPhase(state::CardRef card_ref)
		{
			state::CardRef new_card_ref;

			state_.GetCard(card_ref).GetRawData().onplay_handler.OnPlay(state_, flow_context_, state_.GetCurrentPlayerId(), card_ref, &new_card_ref);
			assert(!new_card_ref.IsValid()); // hero power cannot transformed

			state_.TriggerEvent<state::Events::EventTypes::AfterHeroPower>(
				state::Events::EventTypes::AfterHeroPower::Context{ Manipulate(state_, flow_context_), card_ref });

			return true;
		}

		inline bool FlowController::PlaySpellCardPhase(state::CardRef card_ref)
		{
			state::CardRef new_card_ref;
			bool is_secret = state_.GetCard(card_ref).IsSecretCard();

			if (is_secret) {
				if (state_.GetCurrentPlayer().secrets_.Exists(state_.GetCard(card_ref).GetCardId())) {
					// before playing this secret card, we've ensured (by an assert) that there's no such secret
					// but now, there's a same secret now
					// this might due to some pre-triggers
					// However, this should be considered as a valid game action.
					return true;
				}
			}

			state_.GetCard(card_ref).GetRawData().onplay_handler.OnPlay(state_, flow_context_, state_.GetCurrentPlayerId(), card_ref, &new_card_ref);
			assert(!new_card_ref.IsValid()); // spell cannot be transformed

			if (is_secret) {
				state_.GetZoneChanger<state::kCardTypeSpell, state::kCardZoneHand>(card_ref)
					.ChangeTo<state::kCardZonePlay>(state_.GetCurrentPlayerId());
				state_.GetCurrentPlayer().secrets_.Add(state_.GetCard(card_ref).GetCardId(), card_ref);
			}
			else {
				state_.GetZoneChanger<state::kCardTypeSpell, state::kCardZoneHand>(card_ref)
					.ChangeTo<state::kCardZoneGraveyard>(state_.GetCurrentPlayerId());
			}

			state_.TriggerEvent<state::Events::EventTypes::AfterSpellPlayed>(
				state::Events::EventTypes::AfterSpellPlayed::Context{ Manipulate(state_, flow_context_), state_.GetCurrentPlayerId(), card_ref });
			if (is_secret) {
				state_.TriggerEvent<state::Events::EventTypes::AfterSecretPlayed>(
					state::Events::EventTypes::AfterSecretPlayed::Context{ Manipulate(state_, flow_context_), state_.GetCurrentPlayerId(), card_ref });
			}
			return true;
		}

		inline void FlowController::AttackInternal(state::CardRef attacker)
		{
			if (!AttackPhase(attacker)) return;
			if (!flow_context_.GetResolver().Resolve(state_, flow_context_)) return;
		}

		inline bool FlowController::AttackPhase(state::CardRef attacker)
		{
			assert(attacker.IsValid());
			assert(ValidActionGetter(state_).IsAttackable(attacker));

			state::CardRef defender = GetDefender(attacker);
			if (!defender.IsValid()) return SetInvalid();

			state_.TriggerEvent<state::Events::EventTypes::PrepareAttackTarget>(
				state::Events::EventTypes::PrepareAttackTarget::Context{
				Manipulate(state_, flow_context_), attacker, &defender });

			// do resolve here since some enchantments might be added in 'PrepareAttackTarget' event
			if (!flow_context_.GetResolver().Resolve(state_, flow_context_)) return false;

			if (!defender.IsValid()) return engine::kResultNotDetermined;
			if (state_.GetCardsManager().Get(defender).GetHP() <= 0) return true;
			if (state_.GetCardsManager().Get(defender).GetZone() != state::kCardZonePlay) return true;

			state_.TriggerEvent<state::Events::EventTypes::BeforeAttack>(
				state::Events::EventTypes::BeforeAttack::Context{ Manipulate(state_, flow_context_), attacker, defender });

			// a minion might be destroyed during BeforeAttack event
			if (!flow_context_.GetResolver().Resolve(state_, flow_context_)) return false;
			if (state_.GetCard(attacker).GetZone() != state::kCardZonePlay) return true;
			if (state_.GetCard(defender).GetZone() != state::kCardZonePlay) return true;

			Manipulate(state_, flow_context_).OnBoardCharacter(attacker).Stealth(false);
			Manipulate(state_, flow_context_).OnBoardCharacter(defender)
				.Damage(attacker, state_.GetCardAttackConsiderWeapon(attacker));
			Manipulate(state_, flow_context_).OnBoardCharacter(attacker)
				.Damage(defender, state_.GetCard(defender).GetAttack());

			state_.GetMutableCard(attacker).IncreaseNumAttacksThisTurn();

			{
				const state::Cards::Card & attacker_card = state_.GetCardsManager().Get(attacker);
				if (attacker_card.GetCardType() == state::kCardTypeHero) {
					state::CardRef weapon_ref = state_.GetBoard().Get(attacker_card.GetPlayerIdentifier()).GetWeaponRef();
					if (weapon_ref.IsValid()) {
						Manipulate(state_, flow_context_).Weapon(weapon_ref).Damage(attacker, 1);
					}
				}
			}

			state_.TriggerEvent<state::Events::EventTypes::AfterAttack>(
				state::Events::EventTypes::AfterAttack::Context{ Manipulate(state_, flow_context_), attacker, defender });

			return true;
		}

		inline state::CardRef FlowController::GetDefender(state::CardRef attacker)
		{
			std::vector<int> defenders;

			auto side = state::OppositePlayerSide(state_.GetCurrentPlayerId().GetSide());
			auto const& player = state_.GetBoard().Get(side);

			{
				auto HasTaunt = [&](state::CardRef card_ref) {
					state::Cards::Card const& card = state_.GetCard(card_ref);
					if (!card.HasTaunt()) return false;
					if (card.HasStealth()) return false; // stealth overrides taunt
					if (card.GetImmune()) return false; // immune overrides taunt
					return true;
				};

				int target_idx = ActionTargetIndex::GetIndexForMinion(side, 0);
				player.minions_.ForEach([&](state::CardRef card_ref) {
					if (HasTaunt(card_ref)) defenders.push_back(target_idx);
					++target_idx;
					return true;
				});
				// TODO: hero does not have taunt unless in tavern brawl. so we skip the check here
				assert(!HasTaunt(player.GetHeroRef()));
			}

			if (defenders.empty()) {
				// no taunt, all characters can be the target

				bool cant_attack_hero = state_.GetCardBoolAttributeConsiderWeapon(attacker, [](state::Cards::Card const& card) {
					return card.IsCantAttackHero();
				});

				if (!cant_attack_hero) {
					defenders.push_back(ActionTargetIndex::GetIndexForHero(side));
				}

				int target_idx = ActionTargetIndex::GetIndexForMinion(side, 0);
				player.minions_.ForEach([&](state::CardRef card_ref) {
					[&](state::CardRef card_ref) {
						state::Cards::Card const& card = state_.GetCard(card_ref);
						if (card.HasStealth()) return;
						if (card.GetImmune()) return;
						defenders.push_back(target_idx);
					}(card_ref);
					++target_idx;
					return true;
				});
			}

			if (defenders.empty()) return state::CardRef();
			return flow_context_.GetDefender(defenders);
		}

		inline void FlowController::HeroPowerInternal()
		{
			if (!HeroPowerPhase()) return;
			if (!flow_context_.GetResolver().Resolve(state_, flow_context_)) return;
		}

		inline bool FlowController::HeroPowerPhase()
		{
			state::CardRef card_ref = state_.GetCurrentPlayer().GetHeroPowerRef();

			assert(state_.GetCard(card_ref).GetPlayerIdentifier() == state_.GetCurrentPlayerId());
			assert(state_.GetCard(card_ref).GetCardType() == state::kCardTypeHeroPower);

			assert(state_.GetCard(card_ref).GetRawData().usable);

			if (!PlayCardPhase<state::kCardTypeHeroPower>(card_ref)) return false;

			Manipulate(state_, flow_context_).HeroPower(card_ref).IncreaseUsedThisTurn();
			if (state_.GetCard(card_ref).GetRawData().used_this_turn >= GetMaxHeroPowerUseThisTurn()) {
				Manipulate(state_, flow_context_).HeroPower(card_ref).SetUnusable();
			}

			return true;
		}

		inline int FlowController::GetMaxHeroPowerUseThisTurn()
		{
			// TODO: consider other enchantment to increase this
			return 1;
		}

		inline void FlowController::EndTurnInternal()
		{
			if (state_.GetTurn() >= 89) {
				flow_context_.SetResult(engine::kResultDraw);
				return;
			}
			state_.IncreaseTurn();

			flow_context_.Reset();
			EndTurnPhase();
			if (!flow_context_.GetResolver().Resolve(state_, flow_context_)) return;

			state_.GetMutableCurrentPlayerId().ChangeSide();

			flow_context_.Reset();
			StartTurnPhase();
			if (!flow_context_.GetResolver().Resolve(state_, flow_context_)) return;

			flow_context_.Reset();
			DrawCardPhase();
			if (!flow_context_.GetResolver().Resolve(state_, flow_context_)) return;
		}

		inline void FlowController::EndTurnPhase()
		{
			state_.TriggerEvent<state::Events::EventTypes::OnTurnEnd>(
				state::Events::EventTypes::OnTurnEnd::Context{ Manipulate(state_, flow_context_) });

			// thaw freezed characters
			auto thaw_op = [&](state::CardRef card_ref) {
				state::Cards::Card const& card = state_.GetCard(card_ref);

				if (!card.GetRawData().freezed) return true;
				if (card.GetRawData().num_attacks_this_turn > 0) return true;
				if (card.GetCardType() == state::kCardTypeMinion) {
					if (card.HasCharge() == false && card.GetRawData().just_played) return true;
				}

				Manipulate(state_, flow_context_).OnBoardCharacter(card_ref).Freeze(false);
				return true;
			};
			thaw_op(state_.GetBoard().GetFirst().GetHeroRef());
			thaw_op(state_.GetBoard().GetSecond().GetHeroRef());
			state_.GetBoard().GetFirst().minions_.ForEach(thaw_op);
			state_.GetBoard().GetSecond().minions_.ForEach(thaw_op);

			state_.GetBoard().GetFirst().EndTurn();
			state_.GetBoard().GetSecond().EndTurn();
		}

		inline void FlowController::StartTurnPhase()
		{
			state_.GetCurrentPlayer().GetResource().TurnStart();

			state_.TriggerEvent<state::Events::EventTypes::OnTurnStart>(
				state::Events::EventTypes::OnTurnStart::Context{ Manipulate(state_, flow_context_) });

			for (state::CardRef minion : state_.GetCurrentPlayer().minions_.Get()) {
				Manipulate(state_, flow_context_).OnBoardMinion(minion).TurnStart();
			}
			Manipulate(state_, flow_context_).CurrentHero().TurnStart();
			Manipulate(state_, flow_context_).HeroPower(state_.GetCurrentPlayerId()).TurnStart();
		}

		inline void FlowController::DrawCardPhase()
		{
			Manipulate(state_, flow_context_).CurrentPlayer().DrawCard();
		}

		inline bool FlowController::SetResult(engine::Result result)
		{
			assert(result != engine::kResultNotDetermined);
			flow_context_.SetResult(result);
			return false;
		}

	}
}