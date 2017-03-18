#pragma once

#include "state/Types.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace FlowControl
{
	class FlowContext;

	namespace flag_aura
	{
		class Handler;

		namespace contexts {
			struct AuraIsValid
			{
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
				flag_aura::Handler const& aura_handler_;
			};

			struct AuraApplyRemove
			{
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
				flag_aura::Handler & handler_;
			};
			
			typedef AuraApplyRemove AuraApply;
			typedef AuraApplyRemove AuraRemove;
		}
	}
}