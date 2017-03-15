#pragma once

#include "FlowControl/Contexts.h"
#include "FlowControl/FlowContext.h"
#include "state/State.h"

namespace FlowControl
{
	namespace Context
	{
		inline state::CardRef BattleCry::GetTarget() {
			state::CardRef target = flow_context_.battlecry_target_;
			assert(target.IsValid());
			return target;
		}
	}
}