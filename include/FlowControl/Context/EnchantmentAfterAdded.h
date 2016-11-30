#pragma once

#include <functional>
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
		class EnchantmentAfterAdded
		{
		public:
			EnchantmentAfterAdded(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Enchantments::ContainerType::Identifier enchant_id)
				: state_(state), flow_context_(flow_context), card_ref_(card_ref), enchant_id_(enchant_id)
			{
			}

			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			state::Cards::Enchantments::ContainerType::Identifier enchant_id_;
		};
	}
}