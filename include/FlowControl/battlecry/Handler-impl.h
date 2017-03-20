#pragma once

#include "FlowControl/battlecry/Handler.h"

#include "FlowControl/battlecry/Contexts-impl.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace battlecry
	{
		inline void Handler::DoBattlecry(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const
		{
			if (!battlecry) return;
			(*battlecry)({ state, flow_context, card_ref, card });
		}
	}
}