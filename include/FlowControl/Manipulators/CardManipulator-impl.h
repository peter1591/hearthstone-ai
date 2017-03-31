#pragma once
#include "FlowControl/Manipulators/CardManipulator.h"

#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void CardManipulator::AfterAddedToPlayZone() {
			card_.SetJustPlayedFlag(true);
			card_.GetRawData().added_to_play_zone(state::Cards::ZoneChangedContext{ Manipulate(state_, flow_context_), card_ref_, card_ });
		}
	}
}