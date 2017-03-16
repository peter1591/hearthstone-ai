#pragma once

#include <assert.h>
#include "FlowControl/battlecry/Contexts.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace battlecry
	{
		namespace context
		{
			inline state::CardRef BattleCry::GetTarget() {
				state::CardRef target = flow_context_.battlecry_target_;
				assert(target.IsValid());
				return target;
			}
		}
	}
}