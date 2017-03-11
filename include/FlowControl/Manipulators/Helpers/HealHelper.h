#pragma once

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class HealHelper
			{
			public:
				HealHelper(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, int amount);
			};
		}
	}
}