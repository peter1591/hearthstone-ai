#pragma once

#include "FlowControl/FlowController.h"

#include "FlowControl/ValidActionGetter.h"
#include "FlowControl/Manipulate-impl.h"

namespace FlowControl
{
	inline Result FlowController::PlayCard(int hand_idx)
	{
		flow_context_.ResetActionParameter();
		PlayCardInternal(hand_idx);

		assert(flow_context_.Empty());
		return flow_context_.GetResult();
	}

	inline Result FlowController::EndTurn()
	{
		flow_context_.ResetActionParameter();
		EndTurnInternal();

		assert(flow_context_.Empty());
		return flow_context_.GetResult();
	}

	inline Result FlowController::Attack(state::CardRef attacker, state::CardRef defender)
	{
		flow_context_.ResetActionParameter();
		AttackInternal(attacker, defender);

		assert(flow_context_.Empty());
		return flow_context_.GetResult();
	}

	inline Result FlowController::HeroPower()
	{
		flow_context_.ResetActionParameter();
		HeroPowerInternal();

		assert(flow_context_.Empty());
		return flow_context_.GetResult();
	}

	inline Result FlowController::Resolve()
	{
		detail::Resolver(state_, flow_context_).Resolve();
		return flow_context_.GetResult();
	}

	inline void FlowController::PlayCardInternal(int hand_idx)
	{
		state::CardRef card_ref = state_.GetCurrentPlayer().hand_.Get(hand_idx);

		switch (state_.GetCardsManager().Get(card_ref).GetCardType())
		{
		case state::kCardTypeMinion:
			PlayCardPhase<state::kCardTypeMinion>(card_ref);
			break;
		case state::kCardTypeWeapon:
			PlayCardPhase<state::kCardTypeWeapon>(card_ref);
			break;
		case state::kCardTypeSpell:
			PlayCardPhase<state::kCardTypeSpell>(card_ref);
			break;
		default:
			assert(false);
			return;
		}

		if (!detail::Resolver(state_, flow_context_).Resolve()) return;

		++state_.GetCurrentPlayer().played_cards_this_turn_;
	}

	template <state::CardType CardType>
	inline bool FlowController::PlayCardPhase(state::CardRef card_ref)
	{
		state_.IncreasePlayOrder();
		Manipulate(state_, flow_context_).Card(card_ref).SetPlayOrder();

		if (!state_.GetCard(card_ref).GetRawData().onplay_handler.PrepareTarget(state_, flow_context_, state_.GetCurrentPlayerId(), card_ref)) {
			return false;
		}
		state::CardRef onplay_target = flow_context_.GetSpecifiedTarget();
		if (onplay_target.IsValid()) {
			state_.TriggerEvent<state::Events::EventTypes::PreparePlayCardTarget>(state::Events::EventTypes::PreparePlayCardTarget::Context{
				Manipulate(state_, flow_context_), card_ref, &onplay_target
			});
			assert(onplay_target.IsValid());
			flow_context_.ChangeSpecifiedTarget(onplay_target);
		}

		int cost = state_.GetCard(card_ref).GetCost();
		bool cost_health_instead = false;

		state_.TriggerEvent<state::Events::EventTypes::GetPlayCardCost>(state::Events::EventTypes::GetPlayCardCost::Context{
			Manipulate(state_, flow_context_), card_ref, &cost, &cost_health_instead
		});

		if (cost_health_instead) {
			if (!CostHealth(cost)) return false;
		}
		else {
			if (!CostCrystal(cost)) return false;
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

	inline bool FlowController::CostCrystal(int amount) {
		if (amount < 0) amount = 0;

		auto& crystal = state_.GetCurrentPlayer().GetResource();
		if (crystal.GetCurrent() < amount) {
			return SetResult(FlowControl::kResultInvalid);
		}
		crystal.Cost(amount);
		return true;
	}

	inline bool FlowController::CostHealth(int amount) {
		state::CardRef hero_ref = state_.GetCurrentPlayer().GetHeroRef();
		if (state_.GetCard(hero_ref).GetHP() < amount) {
			return SetResult(FlowControl::kResultInvalid);
		}
		Manipulate(state_, flow_context_).Hero(hero_ref).Damage(hero_ref, amount);
		return true;
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

		if (state_.GetCurrentPlayer().minions_.Full()) return SetResult(kResultInvalid);

		int total_minions = (int)state_.GetCurrentPlayer().minions_.Size();
		int put_position = flow_context_.GetMinionPutLocation(0, total_minions);

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
		[&](){
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
			if (state_.GetCurrentPlayer().secrets_.Exists(state_.GetCard(card_ref).GetCardId()))
				return SetResult(FlowControl::kResultInvalid);
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

	inline void FlowController::AttackInternal(state::CardRef attacker, state::CardRef defender)
	{
		if (!AttackPhase(attacker, defender)) return;
		if (!detail::Resolver(state_, flow_context_).Resolve()) return;
	}

	inline bool FlowController::AttackPhase(state::CardRef attacker, state::CardRef defender)
	{
		if (!attacker.IsValid()) return SetResult(kResultInvalid);
		if (!defender.IsValid()) return SetResult(kResultInvalid);

		assert(state_.GetCardsManager().Get(attacker).GetHP() > 0);
		assert(state_.GetCardsManager().Get(attacker).GetZone() == state::kCardZonePlay);

		if (!IsAttackable(attacker)) return SetResult(kResultInvalid);
		if (!IsDefendable(defender)) return SetResult(kResultInvalid);

		if (state_.GetCard(defender).GetCardType() == state::kCardTypeHero) {
			bool cant_attack_hero = state_.GetCardBoolAttributeConsiderWeapon(attacker, [](state::Cards::Card const& card) {
				return card.IsCantAttackHero();
			});
			if (cant_attack_hero) return SetResult(kResultInvalid);
		}

		state_.TriggerEvent<state::Events::EventTypes::PrepareAttackTarget>(
			state::Events::EventTypes::PrepareAttackTarget::Context{
			Manipulate(state_, flow_context_), attacker, &defender });

		// do resolve here since some enchantments might be added in 'PrepareAttackTarget' event
		if (!detail::Resolver(state_, flow_context_).Resolve()) return false;

		if (!defender.IsValid()) return kResultNotDetermined;
		if (state_.GetCardsManager().Get(defender).GetHP() <= 0) return SetResult(kResultNotDetermined);
		if (state_.GetCardsManager().Get(defender).GetZone() != state::kCardZonePlay) return SetResult(kResultNotDetermined);

		state_.TriggerEvent<state::Events::EventTypes::BeforeAttack>(
			state::Events::EventTypes::BeforeAttack::Context{ Manipulate(state_, flow_context_), attacker, defender });
		
		// a minion might be destroyed during BeforeAttack event
		if (!detail::Resolver(state_, flow_context_).Resolve()) return false;
		if (state_.GetCard(attacker).GetZone() != state::kCardZonePlay) return SetResult(kResultNotDetermined);
		if (state_.GetCard(defender).GetZone() != state::kCardZonePlay) return SetResult(kResultNotDetermined);

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

	inline bool FlowController::IsAttackable(state::CardRef attacker)
	{
		return ValidActionGetter(state_).IsAttackable(attacker);
	}

	inline bool FlowController::IsDefendable(state::CardRef defender)
	{
		state::Cards::Card const& card = state_.GetCard(defender);
		assert(card.GetCardType() == state::kCardTypeHero ||
			card.GetCardType() == state::kCardTypeMinion);

		auto HasTaunt = [&](state::CardRef card_ref) {
			state::Cards::Card const& card = state_.GetCard(card_ref);
			if (!card.HasTaunt()) return false;
			if (card.HasStealth()) return false; // stealth overrides taunt
			if (card.GetImmune()) return false; // immune overrides taunt
			return true;
		};

		if (HasTaunt(defender)) return true;
		
		for (state::CardRef minion : state_.GetBoard().Get(card.GetPlayerIdentifier()).minions_.Get()) {
			if (HasTaunt(minion)) return SetResult(FlowControl::kResultInvalid);
		}
		return true;
	}

	inline void FlowController::HeroPowerInternal()
	{
		if (!HeroPowerPhase()) return;
		if (!detail::Resolver(state_, flow_context_).Resolve()) return;
	}

	inline bool FlowController::HeroPowerPhase()
	{
		state::CardRef card_ref = state_.GetCurrentPlayer().GetHeroPowerRef();
		state::Cards::Card const& card = state_.GetCard(card_ref);

		assert(card.GetPlayerIdentifier() == state_.GetCurrentPlayerId());
		assert(card.GetCardType() == state::kCardTypeHeroPower);

		if (!card.GetRawData().usable) return SetResult(FlowControl::kResultInvalid);

		if (!PlayCardPhase<state::kCardTypeHeroPower>(card_ref)) return false;

		Manipulate(state_, flow_context_).HeroPower(card_ref).IncreaseUsedThisTurn();
		if (card.GetRawData().used_this_turn >= GetMaxHeroPowerUseThisTurn()) {
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
			flow_context_.SetResult(kResultDraw);
			return;
		}
		state_.IncreaseTurn();

		flow_context_.ResetActionParameter();
		EndTurnPhase();
		if (!detail::Resolver(state_, flow_context_).Resolve()) return;

		state_.GetMutableCurrentPlayerId().ChangeSide();

		flow_context_.ResetActionParameter();
		StartTurnPhase();
		if (!detail::Resolver(state_, flow_context_).Resolve()) return;

		flow_context_.ResetActionParameter();
		DrawCardPhase();
		if (!detail::Resolver(state_, flow_context_).Resolve()) return;
	}

	inline void FlowController::EndTurnPhase()
	{
		state_.TriggerEvent<state::Events::EventTypes::OnTurnEnd>(
			state::Events::EventTypes::OnTurnEnd::Context{ Manipulate(state_, flow_context_) });

		// thaw freezed characters
		auto thaw_op = [&](state::CardRef card_ref) {
			state::Cards::Card const& card = state_.GetCard(card_ref);

			if (!card.GetRawData().freezed) return;
			if (card.GetRawData().num_attacks_this_turn > 0) return;
			if (card.GetCardType() == state::kCardTypeMinion) {
				if (card.HasCharge() == false && card.GetRawData().just_played) return;
			}

			Manipulate(state_, flow_context_).OnBoardCharacter(card_ref).Freeze(false);
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

	inline bool FlowController::SetResult(Result result)
	{
		assert(result != kResultNotDetermined);
		flow_context_.SetResult(result);
		return false;
	}

}