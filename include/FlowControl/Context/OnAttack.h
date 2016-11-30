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
	namespace Context
	{
		struct OnAttack
		{
			OnAttack(state::State & state, FlowContext & flow_context, state::CardRef attacker, state::CardRef defender)
				: state_(state), flow_context_(flow_context), attacker_(attacker), defender_(defender)
			{
			}

			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef attacker_;
			state::CardRef defender_;
		};
	}
}