#pragma once
#include "FlowControl/Manipulate.h"

#include "state/State.h"
#include "state/FlowContext.h"

namespace FlowControl
{
	inline Manipulate::Manipulate(state::State & state, state::FlowContext & flow_context)
		: state_(state), flow_context_(flow_context)
	{
	}

	inline Manipulators::CardManipulator Manipulate::Card(state::CardRef ref)
	{
		return state_.GetCardsManager().GetCardManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::HeroManipulator Manipulate::CurrentHero()
	{
		return Hero(state_.GetCurrentPlayerId());
	}

	inline Manipulators::HeroManipulator Manipulate::OpponentHero()
	{
		return Hero(state_.GetCurrentPlayerId().Opposite());
	}

	inline Manipulators::HeroManipulator Manipulate::Hero(state::PlayerIdentifier player)
	{
		state::CardRef ref = state_.GetBoard().Get(player).hero_ref_;
		assert(state_.GetCardsManager().Get(ref).GetPlayerIdentifier() == player);
		return state_.GetCardsManager().GetHeroManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::HeroManipulator Manipulate::Hero(state::CardRef hero_ref)
	{
		return state_.GetCardsManager().GetHeroManipulator(state_, flow_context_, hero_ref);
	}

	inline Manipulators::MinionManipulator Manipulate::Minion(state::CardRef ref)
	{
		return state_.GetCardsManager().GetMinionManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::CharacterManipulator Manipulate::Character(state::CardRef ref)
	{
		return state_.GetCardsManager().GetCharacterManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::WeaponManipulator Manipulate::Weapon(state::CardRef ref)
	{
		return state_.GetCardsManager().GetWeaponManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::BoardManipulator Manipulate::Board()
	{
		return Manipulators::BoardManipulator(state_, flow_context_);
	}
}