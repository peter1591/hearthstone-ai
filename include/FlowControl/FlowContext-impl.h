#pragma once

#include "FlowControl/detail/Resolver-impl.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl {
	inline void FlowContext::PrepareSpecifiedTarget(
		state::State & state, state::CardRef card_ref, state::targetor::Targets const & target_info)
	{
		assert(!specified_target_.IsValid());
		targets_.clear();
		target_info.Fill(state, targets_);

		if (targets_.empty()) {
			specified_target_.Invalidate();
			return;
		}

		specified_target_ = action_parameters_->GetSpecifiedTarget(state, card_ref, targets_);
		assert(specified_target_.IsValid());
	}

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