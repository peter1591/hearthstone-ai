#pragma once
#include "FlowControl/Manipulators/CardManipulator.h"

#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void CardManipulator::AfterAddedToPlayZone(state::Events::Manager & event_mgr) {
			GetCard().SetJustPlayedFlag(true);
			GetCard().GetRawData().added_to_play_zone(state::Cards::ZoneChangedContext{ Manipulate(state_, flow_context_), event_mgr, card_ref_ });
			// TODO: trigger callback for event-hooked enchantments
		}
		inline state::Cards::Card & CardManipulator::GetCard()
		{
			return state_.GetMutableCard(card_ref_);
		}
	}
}