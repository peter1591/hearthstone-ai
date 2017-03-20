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

	namespace spell
	{
		namespace context
		{
			struct DoSpell
			{
				state::State & state_;
				FlowContext & flow_context_;
				state::CardRef const card_ref_;
				const state::Cards::Card & card_;

				state::CardRef GetTarget();
			};

			struct TargetGetter
			{
				state::State & state_;
				FlowContext & flow_context_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
			};
		}
	}
}