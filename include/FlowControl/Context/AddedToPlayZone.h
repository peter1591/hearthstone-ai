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
		struct AddedToPlayZone
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};
	}
}