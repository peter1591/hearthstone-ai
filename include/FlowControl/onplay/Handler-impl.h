#pragma once

#include "FlowControl/onplay/Handler.h"

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

			bool need_to_prepare_target = true;
			bool allow_no_target = true;
			state::targetor::Targets targets;
			
			if (!(*specified_target_getter)(
				context::GetSpecifiedTarget{ Manipulate(state, flow_context), player, card_ref, &targets, &allow_no_target, &need_to_prepare_target }))
			{
				return false;
			}
			if (!need_to_prepare_target) return true;

			return flow_context.PrepareSpecifiedTarget(state, card_ref, targets, allow_no_target);
		}

		inline void Handler::OnPlay(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref, state::CardRef * new_card_ref) const
		{
			if (!onplay) return;
			(*onplay)({ Manipulate(state, flow_context), player, card_ref, new_card_ref });
		}
	}
}