#pragma once

#include "FlowControl/Dispatchers/Minions.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class FlagAuraHelper
			{
			public:
				FlagAuraHelper(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) :
					state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{
				}

				// return a boolean flag indicates if the aura is still valid
				bool Update() {
					return card_.GetMutableFlagAuraHandlerGetter().Get().Update(state_, flow_context_, card_ref_, card_);
				}

			private:
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};
		}
	}
}