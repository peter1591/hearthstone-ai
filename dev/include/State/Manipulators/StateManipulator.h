#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Manipulators/CharacterManipulator.h"
#include "State/Manipulators/CardManipulator.h"
#include "State/Manipulators/MinionManipulator.h"

namespace State
{
	namespace Manipulators
	{
		class StateManipulator
		{
		public:
			StateManipulator(State & state) : state_(state)
			{

			}

			CharacterManipulator CurrentHero()
			{
				return Hero(state_.current_player);
			}

			CharacterManipulator Hero(PlayerIdentifier player)
			{
				CardRef ref = state_.board.players.Get(player).hero_ref_;
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
			State & state_;
		};
	}
}