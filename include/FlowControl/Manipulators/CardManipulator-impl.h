#pragma once
#include "FlowControl/Manipulators/CardManipulator.h"

#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void CardManipulator::AfterAddedToPlayZone(state::Events::Manager & event_mgr) {
			GetCard().SetJustPlayedFlag(true);
			GetCard().GetRawData().added_to_play_zone(state::Cards::ZoneChangedContext
				{ state_, card_ref_ });
		}
		inline void CardManipulator::AfterAddedToHandZone(state::Events::Manager & event_mgr) {
			GetCard().GetRawData().added_to_hand_zone(state::Cards::ZoneChangedContext{ state_, card_ref_ });
		}
		inline state::Cards::Card & CardManipulator::GetCard()
		{
			return state_.GetMutableCard(card_ref_);
		}
	}
}