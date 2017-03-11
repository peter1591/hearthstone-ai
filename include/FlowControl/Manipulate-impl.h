#pragma once
#include "FlowControl/Manipulate.h"

#include "State/State.h"

namespace FlowControl
{
	inline Manipulators::CardManipulator Manipulate::Card(state::CardRef ref)
	{
		return Manipulators::CardManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref));
	}

	inline Manipulators::HeroManipulator Manipulate::CurrentHero()
	{
		return Hero(state_.current_player);
	}

	inline Manipulators::HeroManipulator Manipulate::OpponentHero()
	{
		return AnotherHero(state_.current_player);
	}

	inline Manipulators::HeroManipulator Manipulate::Hero(state::CardRef ref)
	{
		state::PlayerIdentifier player = state_.mgr.Get(ref).GetPlayerIdentifier();
		assert(state_.board.Get(player).hero_ref_ == ref);
		return Manipulators::HeroManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref), player);
	}

	inline Manipulators::HeroManipulator Manipulate::Hero(state::PlayerIdentifier player)
	{
		state::CardRef ref = state_.board.Get(player).hero_ref_;
		return Manipulators::HeroManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref), player);
	}

	inline Manipulators::HeroManipulator Manipulate::AnotherHero(state::PlayerIdentifier player)
	{
		return Hero(player.Opposite());
	}

	inline Manipulators::MinionManipulator Manipulate::Minion(state::CardRef ref)
	{
		return Manipulators::MinionManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref));
	}

	inline Manipulators::CharacterManipulator Manipulate::Character(state::CardRef ref)
	{
		return Manipulators::CharacterManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref));
	}

	inline Manipulators::WeaponManipulator Manipulate::Weapon(state::CardRef ref)
	{
		return Manipulators::WeaponManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref));
	}

	inline Manipulators::BoardManipulator Manipulate::Board()
	{
		return Manipulators::BoardManipulator(state_, flow_context_);
	}
}