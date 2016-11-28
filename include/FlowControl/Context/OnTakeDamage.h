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
		struct OnTakeDamage
		{
			OnTakeDamage(state::State & state, state::CardRef card_ref, int damage)
				: state_(state), card_ref_(card_ref), damage_(damage)
			{
			}

			state::State & state_;
			state::CardRef card_ref_;
			int damage_;
		};
	}
}