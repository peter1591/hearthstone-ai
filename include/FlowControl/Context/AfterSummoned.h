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
		class AfterSummoned
		{
		public:
			AfterSummoned(state::State & state, state::CardRef card_ref, const state::Cards::Card & card)
				: state_(state), card_ref_(card_ref), card_(card)
			{

			}

			state::State & state_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};
	}
}