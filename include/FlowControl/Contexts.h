#pragma once

#include <functional>
#include <unordered_set>
#include "state/Types.h"

namespace state
{
	class State;
	namespace Cards {
		class Card;
		namespace aura { class AuraAuxData; }
	}
	namespace targetor {
		class TargetsGenerator;
	}
}

namespace FlowControl
{
	class FlowContext;
	namespace Context
	{
		struct Deathrattle
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};
	}
}