#pragma once

#include "state/Types.h"
#include "state/targetor/Targets.h"
#include "FlowControl/enchantment/Enchantments.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace FlowControl
{
	class FlowContext;

	namespace aura
	{
		class Handler;

		namespace contexts {
			struct AuraIsValid
			{
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
				aura::Handler const& aura_handler_;
				bool & need_update_;
			};

			struct AuraGetTargets
			{
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
				aura::Handler & aura_handler_;
			};

			struct AuraApplyOn
			{
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
				state::CardRef target_;
			};

			struct AuraRemoveFrom
			{
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				const state::CardRef card_ref_;
				const state::Cards::Card & card_;
				const state::CardRef target_;
				const enchantment::Enchantments::ContainerType::Identifier enchant_id_;
			};
		}
	}
}