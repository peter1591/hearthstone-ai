#pragma once

#include <assert.h>
#include "FlowControl/onplay/Contexts.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace onplay
	{
		namespace context
		{
			inline state::CardRef OnPlay::GetTarget() {
				state::CardRef target = flow_context_.GetSpecifiedTarget();
				assert(target.IsValid());
				return target;
			}
		}
	}
}