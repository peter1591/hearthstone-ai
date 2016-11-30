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
			OnAttack(state::State & state, state::CardRef attacker, state::CardRef defender)
				: state_(state), attacker_(attacker), defender_(defender)
			{
			}

			state::State & state_;
			state::CardRef attacker_;
			state::CardRef defender_;
		};
	}
}