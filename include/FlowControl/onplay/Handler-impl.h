#pragma once

#include "FlowControl/onplay/Handler.h"

#include "FlowControl/onplay/Contexts-impl.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace onplay
	{
		inline void Handler::PrepareTarget(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const
		{
			if (!specified_target_getter) return;

			state::targetor::Targets targets = (*specified_target_getter)(
				context::GetSpecifiedTarget{ state, flow_context, card_ref, card });
			flow_context.PrepareSpecifiedTarget(state, card_ref, card, targets);
		}

		inline void Handler::OnPlay(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const
		{
			if (!onplay) return;
			(*onplay)({ state, flow_context, card_ref, card });
		}
	}
}