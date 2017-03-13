#pragma once

#include "state/Cards/aura/AuraAuxData.h"
#include "FlowControl/Dispatchers/Minions.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class AuraHelper
			{
			public:
				AuraHelper(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) :
					state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{
				}

				void Update();

			private:
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};
		}
	}
}