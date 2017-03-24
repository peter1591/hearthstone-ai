#pragma once

#include "state/Types.h"
#include "state/targetor/TargetsGenerator.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace FlowControl
{
	class FlowContext;

	namespace onplay
	{
		namespace context
		{
			struct GetSpecifiedTarget {
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
			};

			struct OnPlay
			{
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
				state::CardRef * new_card_ref;
				state::Cards::Card const* * new_card;

				state::CardRef GetTarget();
			};
		}
	}
}