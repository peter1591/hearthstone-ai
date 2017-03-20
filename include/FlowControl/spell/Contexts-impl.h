#pragma once

#include <assert.h>
#include "FlowControl/spell/Contexts.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace spell
	{
		namespace context
		{
			inline state::CardRef DoSpell::GetTarget() {
				state::CardRef target = flow_context_.GetSpecifiedTarget();
				assert(target.IsValid()); // TODO: maybe use this to indicate 'cast a spell with a random-choose target'
				return target;
			}
		}
	}
}