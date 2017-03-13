#pragma once

#include "state/FlowContext.h"

namespace state
{
	inline void FlowContext::AddDeadEntryHint(State & state, CardRef ref)
	{
		int play_order = state.GetCardsManager().Get(ref).GetPlayOrder();

		dead_entity_hints_.insert(std::make_pair(play_order, ref));
	}
}