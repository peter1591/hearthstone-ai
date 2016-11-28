#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "FlowControl/Manipulators/CharacterManipulator.h"
#include "FlowControl/Manipulators/MinionManipulator.h"
#include "FlowControl/Manipulators/HeroManipulator.h"
#include "FlowControl/Manipulators/WeaponManipulator.h"

namespace FlowControl
{
	class Manipulate
	{
	public:
		Manipulate(state::State & state) : state_(state) { }

		Manipulators::CardManipulator Card(state::CardRef ref)
		{
			return Manipulators::CardManipulator(state_, ref, state_.mgr.GetMutable(ref));
		}

		Manipulators::HeroManipulator CurrentHero()
		{
			return Hero(state_.current_player);
		}

		Manipulators::HeroManipulator Hero(state::PlayerIdentifier player)
		{
			state::CardRef ref = state_.board.Get(player).hero_ref_;
			return Manipulators::HeroManipulator(state_, ref, state_.mgr.GetMutable(ref));
		}

		Manipulators::MinionManipulator Minion(state::CardRef ref)
		{
			return Manipulators::MinionManipulator(state_, ref, state_.mgr.GetMutable(ref));
		}

		Manipulators::CharacterManipulator Character(state::CardRef ref)
		{
			return Manipulators::CharacterManipulator(state_, ref, state_.mgr.GetMutable(ref));
		}

		Manipulators::WeaponManipulator Weapon(state::CardRef ref)
		{
			return Manipulators::WeaponManipulator(state_, ref, state_.mgr.GetMutable(ref));
		}

	private:
		state::State & state_;
	};
}