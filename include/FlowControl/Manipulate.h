#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "FlowControl/Manipulators/HeroManipulator.h"
#include "FlowControl/Manipulators/WeaponManipulator.h"
#include "FlowControl/Manipulators/BoardManipulator.h"

namespace FlowControl
{
	class FlowContext;

	class Manipulate
	{
	public:
		Manipulate(state::State & state, FlowContext & flow_context) : state_(state), flow_context_(flow_context) { }

		Manipulators::CardManipulator Card(state::CardRef ref)
		{
			return Manipulators::CardManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref));
		}

		Manipulators::HeroManipulator CurrentHero()
		{
			return Hero(state_.current_player);
		}

		Manipulators::HeroManipulator OpponentHero()
		{
			return AnotherHero(state_.current_player);
		}

		Manipulators::HeroManipulator Hero(state::CardRef ref)
		{
			state::PlayerIdentifier player = state_.mgr.Get(ref).GetPlayerIdentifier();
			assert(state_.board.Get(player).hero_ref_ == ref);
			return Manipulators::HeroManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref), player);
		}

		Manipulators::HeroManipulator Hero(state::PlayerIdentifier player)
		{
			state::CardRef ref = state_.board.Get(player).hero_ref_;
			return Manipulators::HeroManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref), player);
		}

		Manipulators::HeroManipulator AnotherHero(state::PlayerIdentifier player)
		{
			return Hero(player.Opposite());
		}

		Manipulators::MinionManipulator Minion(state::CardRef ref)
		{
			return Manipulators::MinionManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref));
		}

		Manipulators::CharacterManipulator Character(state::CardRef ref)
		{
			return Manipulators::CharacterManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref));
		}

		Manipulators::WeaponManipulator Weapon(state::CardRef ref)
		{
			return Manipulators::WeaponManipulator(state_, flow_context_, ref, state_.mgr.GetMutable(ref));
		}

		Manipulators::BoardManipulator Board()
		{
			return Manipulators::BoardManipulator(state_, flow_context_);
		}

	private:
		state::State & state_;
		FlowContext & flow_context_;
	};
}