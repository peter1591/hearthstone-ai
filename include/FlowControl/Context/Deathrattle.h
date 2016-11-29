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
		class Deathrattle
		{
		public:
			Deathrattle(state::State & state, FlowContext & flow_context, state::CardRef card_ref, const state::Cards::Card & card)
				: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
			{

			}

			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};
	}
}