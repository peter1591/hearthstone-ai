#pragma once

#include <functional>
#include "State/Types.h"

namespace state
{
	class State;
	namespace Cards
	{
		class Card;
	}
}

namespace FlowControl
{
	class FlowContext;
	namespace Context
	{
		struct OnTurnEnd
		{
			OnTurnEnd(state::State & state, FlowContext & flow_context)
				: state_(state), flow_context_(flow_context)
			{
			}

			state::State & state_;
			FlowContext & flow_context_;
		};
	}
}