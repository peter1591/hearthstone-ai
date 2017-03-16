#pragma once

#include "FlowControl/FlowContext.h"

#include "state/State.h"

namespace FlowControl {
	inline void FlowContext::AddDeadEntryHint(state::State & state, state::CardRef ref)
	{
		int play_order = state.GetCardsManager().Get(ref).GetPlayOrder();

		dead_entity_hints_.insert(std::make_pair(play_order, ref));
	}

	inline bool FlowContext::Empty() const
	{
		if (!dead_entity_hints_.empty()) return false;
		if (destroyed_weapon_.IsValid()) return false;
		return true;
	}
}