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
		class AuraGetTargets
		{
		public:
			AuraGetTargets(state::State & state, FlowContext & flow_context, state::CardRef card_ref, const state::Cards::Card & card, state::Cards::AuraAuxData & aura_data, std::unordered_set<state::CardRef> & targets)
				: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card), aura_data_(aura_data), targets_(targets)
			{
			}

			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
			state::Cards::AuraAuxData & aura_data_;
			std::unordered_set<state::CardRef> & targets_;
		};
	}
}