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
		return state_.mgr.GetCardManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::HeroManipulator Manipulate::CurrentHero()
	{
		return Hero(state_.current_player);
	}

	inline Manipulators::HeroManipulator Manipulate::OpponentHero()
	{
		return Hero(state_.current_player.Opposite());
	}

	inline Manipulators::HeroManipulator Manipulate::Hero(state::PlayerIdentifier player)
	{
		state::CardRef ref = state_.board.Get(player).hero_ref_;
		return state_.mgr.GetHeroManipulator(state_, flow_context_, ref, player);
	}

	inline Manipulators::MinionManipulator Manipulate::Minion(state::CardRef ref)
	{
		return state_.mgr.GetMinionManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::CharacterManipulator Manipulate::Character(state::CardRef ref)
	{
		return state_.mgr.GetCharacterManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::WeaponManipulator Manipulate::Weapon(state::CardRef ref)
	{
		return state_.mgr.GetWeaponManipulator(state_, flow_context_, ref);
	}

	inline Manipulators::BoardManipulator Manipulate::Board()
	{
		return Manipulators::BoardManipulator(state_, flow_context_);
	}
}