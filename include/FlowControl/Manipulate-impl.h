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
		return Manipulators::CardManipulator(state_, flow_context_, ref, state_.GetMutableCard(ref));
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
		state::CardRef ref = state_.GetBoard().Get(player).GetHeroRef();
		assert(state_.GetCardsManager().Get(ref).GetPlayerIdentifier() == player);
		return Manipulators::HeroManipulator(state_, flow_context_, ref, state_.GetMutableCard(ref));
	}

	inline Manipulators::HeroManipulator Manipulate::Hero(state::CardRef hero_ref)
	{
		return Manipulators::HeroManipulator(state_, flow_context_, hero_ref, state_.GetMutableCard(hero_ref));
	}

	inline Manipulators::MinionManipulator Manipulate::Minion(state::CardRef ref)
	{
		return Manipulators::MinionManipulator(state_, flow_context_, ref, state_.GetMutableCard(ref));
	}

	inline Manipulators::CharacterManipulator Manipulate::Character(state::CardRef ref)
	{
		return Manipulators::CharacterManipulator(state_, flow_context_, ref, state_.GetMutableCard(ref));
	}

	inline Manipulators::WeaponManipulator Manipulate::Weapon(state::CardRef ref)
	{
		return Manipulators::WeaponManipulator(state_, flow_context_, ref, state_.GetMutableCard(ref));
	}

	inline Manipulators::BoardManipulator Manipulate::Board()
	{
		return Manipulators::BoardManipulator(state_, flow_context_);
	}
}