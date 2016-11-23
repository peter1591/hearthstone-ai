#pragma once

#include "State/State.h"
#include "State/PlayerIdentifier.h"
#include "Manipulators/CharacterManipulator.h"
#include "Manipulators/CardManipulator.h"
#include "Manipulators/MinionManipulator.h"

namespace Manipulators
{
	class StateManipulator
	{
	public:
		StateManipulator(State::State & state) : state_(state)
		{

		}

		CharacterManipulator CurrentHero()
		{
			return Hero(state_.current_player);
		}

		CharacterManipulator Hero(State::PlayerIdentifier player)
		{
			CardRef ref = state_.players.Get(player).hero_ref_;
			return CharacterManipulator(state_, ref, state_.mgr.GetMutable(ref));
		}

		CardManipulator Card(CardRef ref)
		{
			return CardManipulator(state_, ref, state_.mgr.GetMutable(ref));
		}

		MinionManipulator Minion(CardRef ref)
		{
			return MinionManipulator(state_, ref, state_.mgr.GetMutable(ref));
		}

	private:
		State::State & state_;
	};
}