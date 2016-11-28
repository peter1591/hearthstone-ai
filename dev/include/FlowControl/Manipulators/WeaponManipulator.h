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
			WeaponManipulator(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
			}
		};
	}
}