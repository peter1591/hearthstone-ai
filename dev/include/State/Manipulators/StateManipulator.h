#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Manipulators/CharacterManipulator.h"
#include "State/Manipulators/CardManipulator.h"
#include "State/Manipulators/MinionManipulator.h"
#include "State/Manipulators/WeaponManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class StateManipulator
		{
		public:
			StateManipulator(state::State & state) : state_(state)
			{

			}

			CardManipulator Card(CardRef ref)
			{
				return CardManipulator(state_, ref, state_.mgr.GetMutable(ref));
			}

			CharacterManipulator CurrentHero()
			{
				return Hero(state_.current_player);
			}

			CharacterManipulator Hero(state::PlayerIdentifier player)
			{
				CardRef ref = state_.board.Get(player).hero_ref_;
				return CharacterManipulator(state_, ref, state_.mgr.GetMutable(ref));
			}

			MinionManipulator Minion(CardRef ref)
			{
				return MinionManipulator(state_, ref, state_.mgr.GetMutable(ref));
			}

			CharacterManipulator Character(CardRef ref)
			{
				return CharacterManipulator(state_, ref, state_.mgr.GetMutable(ref));
			}

			WeaponManipulator Weapon(CardRef ref)
			{
				return WeaponManipulator(state_, ref, state_.mgr.GetMutable(ref));
			}

		private:
			state::State & state_;
		};
	}
}