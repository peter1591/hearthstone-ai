#pragma once

#include "FlowControl/spell/Handler.h"

#include "FlowControl/spell/Contexts-impl.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace spell
	{
		inline void Handler::DoSpell(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const
		{
			assert(do_spell);
			(*do_spell)({ state, flow_context, card_ref, card });
		}
	}
}