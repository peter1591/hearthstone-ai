#pragma once
#include "FlowControl/FlowController.h"

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
		state::Cards::Card const& card = state_.GetCardsManager().Get(card_ref);

		switch (card.GetCardType())
		{
		case state::kCardTypeMinion:
			PlayCardPhase<state::kCardTypeMinion>(card_ref, card);
			break;
		case state::kCardTypeWeapon:
			PlayCardPhase<state::kCardTypeWeapon>(card_ref, card);
			break;
		case state::kCardTypeSpell:
			PlayCardPhase<state::kCardTypeSpell>(card_ref, card);
			break;
		case state::kCardTypeSecret:
			PlayCardPhase<state::kCardTypeSecret>(card_ref, card);
			break;
		default:
			assert(false);
			return;
		}

		if (!detail::Resolver(state_, flow_context_).Resolve()) return;
	}

	template <state::CardType CardType>
	inline bool FlowController::PlayCardPhase(state::CardRef card_ref, state::Cards::Card const& card)
	{
		card.GetRawData().onplay_handler.PrepareTarget(state_, flow_context_, card_ref, card);

		state_.GetCurrentPlayer().GetResource().IncreaseNextOverload(card.GetRawData().overload);

		return PlayCardPhaseInternal<CardType>(card_ref, card);
	}

	inline bool FlowController::CostCrystal(int amount) {
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

	template <> inline bool FlowController::PlayCardPhaseInternal<state::kCardTypeMinion>(state::CardRef card_ref, state::Cards::Card const& card)
	{
		assert(card.GetCardType() == state::kCardTypeMinion);
		if (!CostCrystal(card.GetCost())) return false;
		return PlayMinionCardPhase(card_ref, card);
	}

	template <> inline bool FlowController::PlayCardPhaseInternal<state::kCardTypeWeapon>(state::CardRef card_ref, state::Cards::Card const& card)
	{
		assert(card.GetCardType() == state::kCardTypeWeapon);
		if (!CostCrystal(card.GetCost())) return false;
		return PlayWeaponCardPhase(card_ref, card);
	}

	template <> inline bool FlowController::PlayCardPhaseInternal<state::kCardTypeHeroPower>(state::CardRef card_ref, state::Cards::Card const& card)
	{
		assert(card.GetCardType() == state::kCardTypeHeroPower);
		if (!CostCrystal(card.GetCost())) return false;
		return PlayHeroPowerCardPhase(card_ref, card);
	}

	template <> inline bool FlowController::PlayCardPhaseInternal<state::kCardTypeSpell>(state::CardRef card_ref, state::Cards::Card const& card)
	{
		// TODO: shared with secret cards
		assert(card.GetCardType() == state::kCardTypeSpell);

		if (state_.GetCurrentPlayer().GetNextSpellCostHealthThisTurn()) {
			if (!CostHealth(card.GetCost())) return false;
			state_.GetCurrentPlayer().SetNextSpellCostHealthThisTurn(false);
		}
		else {
			if (!CostCrystal(card.GetCost())) return false;
		}

		return PlaySpellCardPhase(card_ref, card);
	}

	template <> inline bool FlowController::PlayCardPhaseInternal<state::kCardTypeSecret>(state::CardRef card_ref, state::Cards::Card const& card)
	{
		// TODO: shared with spell cards
		assert(card.GetCardType() == state::kCardTypeSecret);

		if (state_.GetCurrentPlayer().GetNextSpellCostHealthThisTurn()) {
			if (!CostHealth(card.GetCost())) return false;
			state_.GetCurrentPlayer().SetNextSpellCostHealthThisTurn(false);
		}
		else {
			if (!CostCrystal(card.GetCost())) return false;
		}

		return PlaySecretCardPhase(card_ref, card);
	}

	inline bool FlowController::PlayMinionCardPhase(state::CardRef card_ref, state::Cards::Card const& card)
	{
		state_.TriggerEvent<state::Events::EventTypes::BeforeMinionSummoned>(
			state::Events::EventTypes::BeforeMinionSummoned::Context{ Manipulate(state_, flow_context_), card_ref, card });

		if (state_.GetCurrentPlayer().minions_.Full()) return SetResult(kResultInvalid);

		int total_minions = (int)state_.GetCurrentPlayer().minions_.Size();
		int put_position = flow_context_.GetMinionPutLocation(0, total_minions);

		state_.GetZoneChanger<state::kCardTypeMinion, state::kCardZoneHand>(Manipulate(state_, flow_context_), card_ref)
			.ChangeTo<state::kCardZonePlay>(state_.GetCurrentPlayerId(), put_position);

		state_.TriggerEvent<state::Events::EventTypes::OnMinionPlay>(card);

		assert(card.GetPlayerIdentifier() == state_.GetCurrentPlayerId());
		state::CardRef new_card_ref;
		state::Cards::Card const* new_card = nullptr;
		card.GetRawData().onplay_handler.OnPlay(state_, flow_context_, card_ref, card, &new_card_ref, &new_card);
		if (!new_card_ref.IsValid()) {
			// no transformation
			new_card_ref = card_ref;
			new_card = &card;
		}
		assert(new_card != nullptr);
#ifndef NDEBUG
		[&](){
			if (card_ref == new_card_ref) return;
			// minion transformed
			if (card.GetZone() == state::kCardZonePlay) assert(false);
			if (new_card->GetZone() != state::kCardZonePlay) assert(false);
		}();
#endif

		state_.TriggerEvent<state::Events::EventTypes::AfterMinionPlayed>(
			state::Events::EventTypes::AfterMinionPlayed::Context{ Manipulate(state_, flow_context_), new_card_ref, *new_card });

		state_.TriggerEvent<state::Events::EventTypes::AfterMinionSummoned>(new_card_ref, *new_card);
		Manipulate(state_, flow_context_).Minion(new_card_ref).AfterSummoned();

		return true;
	}

	inline bool FlowController::PlayWeaponCardPhase(state::CardRef card_ref, state::Cards::Card const& card)
	{
		Manipulate(state_, flow_context_).CurrentHero().EquipWeapon<state::kCardZoneHand>(card_ref);

		state::CardRef new_card_ref;
		state::Cards::Card const* new_card = nullptr;

		assert(card.GetPlayerIdentifier() == state_.GetCurrentPlayerId());
		card.GetRawData().onplay_handler.OnPlay(state_, flow_context_, card_ref, card, &new_card_ref, &new_card);
		assert(!new_card_ref.IsValid()); // weapon cannot transformed

		return true;
	}

	inline bool FlowController::PlayHeroPowerCardPhase(state::CardRef card_ref, state::Cards::Card const& card)
	{
		state::CardRef new_card_ref;
		state::Cards::Card const* new_card = nullptr;

		card.GetRawData().onplay_handler.OnPlay(state_, flow_context_, card_ref, card, &new_card_ref, &new_card);
		assert(!new_card_ref.IsValid()); // hero power cannot transformed

		state_.TriggerEvent<state::Events::EventTypes::AfterHeroPower>(
			state::Events::EventTypes::AfterHeroPower::Context{ Manipulate(state_, flow_context_), card_ref, card });


		return true;
	}

	inline bool FlowController::PlaySpellCardPhase(state::CardRef card_ref, state::Cards::Card const& card)
	{
		state::CardRef new_card_ref;
		state::Cards::Card const* new_card = nullptr;

		card.GetRawData().onplay_handler.OnPlay(state_, flow_context_, card_ref, card, &new_card_ref, &new_card);
		assert(!new_card_ref.IsValid()); // spell cannot transformed

		state_.GetZoneChanger<state::kCardTypeSpell, state::kCardZoneHand>(Manipulate(state_, flow_context_), card_ref)
			.ChangeTo<state::kCardZoneGraveyard>(state_.GetCurrentPlayerId());

		state_.TriggerEvent<state::Events::EventTypes::AfterSpell>(
			state::Events::EventTypes::AfterSpell::Context{ Manipulate(state_, flow_context_), card_ref, card });

		return true;
	}

	inline bool FlowController::PlaySecretCardPhase(state::CardRef card_ref, state::Cards::Card const& card)
	{
		state::CardRef new_card_ref;
		state::Cards::Card const* new_card = nullptr;

		if (state_.GetCurrentPlayer().secrets_.Exists(card.GetCardId())) return SetResult(FlowControl::kResultInvalid);

		card.GetRawData().onplay_handler.OnPlay(state_, flow_context_, card_ref, card, &new_card_ref, &new_card);
		assert(!new_card_ref.IsValid()); // secret cannot transformed

		state_.GetZoneChanger<state::kCardTypeSecret, state::kCardZoneHand>(Manipulate(state_, flow_context_), card_ref)
			.ChangeTo<state::kCardZonePlay>(state_.GetCurrentPlayerId());

		state_.TriggerEvent<state::Events::EventTypes::AfterSpell>(
			state::Events::EventTypes::AfterSpell::Context{ Manipulate(state_, flow_context_), card_ref, card });
		// TODO: trigger event 'AfterSecret'

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

		if (!IsAttackable(attacker)) return SetResult(kResultInvalid);
		if (!IsDefendable(defender)) return SetResult(kResultInvalid);

		while (true) {
			state::CardRef origin_defender = defender;
			state_.TriggerEvent<state::Events::EventTypes::BeforeAttack>(attacker, state_, defender);
			state_.TriggerCategorizedEvent<state::Events::EventTypes::BeforeAttack>(attacker, state_, defender);

			if (!attacker.IsValid()) return kResultNotDetermined;
			if (state_.GetCardsManager().Get(attacker).GetHP() <= 0) return kResultNotDetermined;
			if (state_.GetCardsManager().Get(attacker).GetZone() != state::kCardZonePlay) return kResultNotDetermined;

			if (!defender.IsValid()) return kResultNotDetermined;
			if (state_.GetCardsManager().Get(defender).GetHP() <= 0) return kResultNotDetermined;
			if (state_.GetCardsManager().Get(defender).GetZone() != state::kCardZonePlay) return kResultNotDetermined;

			if (origin_defender == defender) break;
		}

		state_.TriggerEvent<state::Events::EventTypes::OnAttack>(
			attacker, state::Events::EventTypes::OnAttack::Context{ Manipulate(state_, flow_context_), defender });
		// TODO: attacker lose stealth

		Manipulate(state_, flow_context_).Character(defender).Damage(defender, GetAttackValue(attacker));
		Manipulate(state_, flow_context_).Character(attacker).Damage(attacker, GetAttackValue(defender));

		Manipulate(state_, flow_context_).Character(attacker).AfterAttack();

		state_.TriggerEvent<state::Events::EventTypes::AfterAttack>(attacker, state_, defender);
		state_.TriggerCategorizedEvent<state::Events::EventTypes::AfterAttack>(attacker, state_, defender);

		{
			const state::Cards::Card & attacker_card = state_.GetCardsManager().Get(attacker);
			if (attacker_card.GetCardType() == state::kCardTypeHero) {
				state::CardRef weapon_ref = state_.GetBoard().Get(attacker_card.GetPlayerIdentifier()).GetWeaponRef();
				if (weapon_ref.IsValid()) {
					Manipulate(state_, flow_context_).Weapon(weapon_ref).Damage(attacker, attacker_card, 1);
				}
			}
		}

		return true;
	}

	inline bool FlowController::IsAttackable(state::CardRef attacker)
	{
		state::Cards::Card const& card = state_.GetCardsManager().Get(attacker);

		if (card.GetCardType() == state::kCardTypeMinion) {
			if (card.HasCharge() == false && card.GetRawData().just_played) return false;
		}

		if (card.GetRawData().num_attacks_this_turn >= 1) return false; // TODO: windfury, etc. An attribute 'max_attacks'?

		return true;
	}

	inline bool FlowController::IsDefendable(state::CardRef defender)
	{
		state::Cards::Card const& card = state_.GetCard(defender);
		assert(card.GetCardType() == state::kCardTypeHero ||
			card.GetCardType() == state::kCardTypeMinion);
		
		bool has_taunt = false;
		bool defender_has_taunt = false;
		state_.GetBoard().Get(card.GetPlayerIdentifier()).minions_.ForEach([&](state::CardRef minion) {
			if (state_.GetCard(minion).HasTaunt()) {
				has_taunt = true;
				if (defender == minion) defender_has_taunt = true;
			}
		});

		if (has_taunt && !defender_has_taunt) return SetResult(FlowControl::kResultInvalid);
		return true;
	}

	inline int FlowController::GetAttackValue(state::CardRef ref)
	{
		int attack = 0;

		state::Cards::Card const& card = state_.GetCardsManager().Get(ref);

		int v1 = card.GetAttack();
		if (v1 > 0) attack += v1;

		if (card.GetCardType() == state::kCardTypeHero) {
			state::CardRef weapon_ref = state_.GetBoard().Get(card.GetPlayerIdentifier()).GetWeaponRef();
			if (weapon_ref.IsValid()) {
				state::Cards::Card const& weapon = state_.GetCardsManager().Get(weapon_ref);
				int v2 = weapon.GetAttack();
				if (v2 > 0) attack += v2;
			}
		}

		return attack;
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

		if (!PlayCardPhase<state::kCardTypeHeroPower>(card_ref, card)) return false;

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
		state_.GetBoard().GetFirst().EndTurn();
		state_.GetBoard().GetSecond().EndTurn();
	}

	inline void FlowController::StartTurnPhase()
	{
		state_.GetCurrentPlayer().GetResource().TurnStart();

		state_.TriggerEvent<state::Events::EventTypes::OnTurnStart>();

		for (state::CardRef minion : state_.GetCurrentPlayer().minions_.Get()) {
			Manipulate(state_, flow_context_).Minion(minion).TurnStart();
		}
		Manipulate(state_, flow_context_).CurrentHero().TurnStart();
		Manipulate(state_, flow_context_).HeroPower(state_.GetCurrentPlayerId()).TurnStart();
	}

	inline void FlowController::DrawCardPhase()
	{
		Manipulate(state_, flow_context_).CurrentHero().DrawCard();
	}

	inline bool FlowController::SetResult(Result result)
	{
		assert(result != kResultNotDetermined);
		flow_context_.SetResult(result);
		return false;
	}

}