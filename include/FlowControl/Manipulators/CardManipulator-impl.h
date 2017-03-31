#pragma once
#include "FlowControl/Manipulators/CardManipulator.h"

#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void CardManipulator::AfterAddedToPlayZone(state::Events::Manager & event_mgr) {
			card_.SetJustPlayedFlag(true);
			card_.GetRawData().added_to_play_zone(state::Cards::ZoneChangedContext{ Manipulate(state_, flow_context_), event_mgr, card_ref_, card_ });
			// TODO: trigger callback for event-hooked enchantments
		}
	}
}