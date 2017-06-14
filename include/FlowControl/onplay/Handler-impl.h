#pragma once

#include "FlowControl/onplay/Handler.h"

#include "FlowControl/onplay/Contexts.h"
#include "FlowControl/onplay/Contexts-impl.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace onplay
	{
		inline bool Handler::PrepareTarget(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref) const
		{
			if (!specified_target_getter) return true;

			context::GetSpecifiedTarget context(state, flow_context, player, card_ref);
			if (!(*specified_target_getter)(context)) {
				return false;
			}

			if (!context.NeedToPrepareTarget()) return true;
			
			state::targetor::Targets targets = context.GetTargets();
			return flow_context.PrepareSpecifiedTarget(state, card_ref, targets, context.IsAllowedNoTarget());
		}

		inline void Handler::OnPlay(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref, state::CardRef * new_card_ref) const
		{
			if (!onplay) return;
			(*onplay)({ Manipulate(state, flow_context), player, card_ref, new_card_ref });
		}
	}
}