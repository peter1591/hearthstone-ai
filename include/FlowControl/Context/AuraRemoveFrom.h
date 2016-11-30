#pragma once

#include <functional>
#include <unordered_set>
#include "State/Types.h"
#include "State/Cards/Enchantments/Enchantments.h"

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
		class AuraRemoveFrom
		{
		public:
			AuraRemoveFrom(state::State & state, FlowContext & flow_context, state::CardRef card_ref, const state::Cards::Card & card,
				state::CardRef target, state::Cards::Enchantments::ContainerType::Identifier enchant_id)
				: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card), target_(target), enchant_id_(enchant_id)
			{
			}

			state::State & state_;
			FlowContext & flow_context_;
			const state::CardRef card_ref_;
			const state::Cards::Card & card_;
			const state::CardRef target_;
			const state::Cards::Enchantments::ContainerType::Identifier enchant_id_;
		};
	}
}