#pragma once

#include "FlowControl/onplay/Handler.h"

#include "FlowControl/onplay/Contexts-impl.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace onplay
	{
		inline bool Handler::PrepareTarget(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const
		{
			if (!specified_target_getter) return true;

			bool allow_no_target = true;
			state::targetor::Targets targets = (*specified_target_getter)(
				context::GetSpecifiedTarget{ Manipulate(state, flow_context), card_ref, card, &allow_no_target });

			return flow_context.PrepareSpecifiedTarget(state, card_ref, card, targets, allow_no_target);
		}

		inline void Handler::OnPlay(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card,
			state::CardRef * new_card_ref, state::Cards::Card const* * new_card) const
		{
			if (!onplay) return;
			(*onplay)({ Manipulate(state, flow_context), card_ref, card, new_card_ref, new_card });
		}
	}
}