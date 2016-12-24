#pragma once

#include <functional>
#include <unordered_set>
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
		struct AuraGetTargets
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
			state::Cards::AuraAuxData & aura_data_;
			std::unordered_set<state::CardRef> & targets_;
		};
	}
}