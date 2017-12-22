#pragma once
#include "engine/FlowControl/Manipulators/CardManipulator.h"

#include "state/State.h"

namespace engine {
	namespace FlowControl
	{
		namespace Manipulators
		{
			inline void CardManipulator::SetPlayOrder()
			{
				GetCard().SetPlayOrder(state_.GetPlayOrder());
			}

			inline state::Cards::Card & CardManipulator::GetCard()
			{
				return state_.GetMutableCard(card_ref_);
			}
		}
	}
}