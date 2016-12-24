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
		struct AuraApplyOn
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
			state::CardRef target_;
			state::Cards::Enchantments::ContainerType::Identifier & enchant_id_;
		};
	}
}