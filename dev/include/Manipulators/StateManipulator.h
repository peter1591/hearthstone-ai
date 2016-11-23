#pragma once

#include "State/State.h"
#include "State/PlayerIdentifier.h"
#include "Manipulators/CharacterManipulator.h"

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

	private:
		State::State & state_;
	};
}