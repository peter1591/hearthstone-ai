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

	namespace battlecry
	{
		namespace context
		{
			struct BattleCry
			{
				state::State & state_;
				FlowContext & flow_context_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;

				state::CardRef GetTarget();
			};

			struct BattlecryTargetGetter // TODO: return targets directly
			{
				state::State & state_;
				FlowContext & flow_context_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
				state::targetor::TargetsGenerator & targets_generator_;

				state::targetor::TargetsGenerator & Targets() { return targets_generator_; }
			};
		}
	}
}