#pragma once

#include "State/Cards/Card.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "State/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class WeaponManipulator : public CardManipulator
		{
		public:
			WeaponManipulator(state::State & state, state::CardRef card_ref, state::Cards::Card &card)
				: CardManipulator(state, card_ref, card)
			{
			}

		public:
			void ReduceDurability(int v) {
				card_.SetDamage(card_.GetDamage() + v);
			}
		};
	}
}