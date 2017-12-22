#pragma once

#include <assert.h>
#include "engine/FlowControl/onplay/Contexts.h"
#include "engine/FlowControl/FlowContext.h"

namespace engine {
	namespace FlowControl
	{
		namespace onplay
		{
			namespace context
			{
				inline state::CardRef OnPlay::GetTarget() const {
					return manipulate_.GetSpecifiedTarget();
				}
			}
		}
	}
}