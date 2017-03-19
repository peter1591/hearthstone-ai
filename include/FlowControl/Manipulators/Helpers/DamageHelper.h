#pragma once

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class DamageHelper
			{
			public:
				DamageHelper(state::State & state, FlowControl::FlowContext & flow_context,
					state::CardRef source_ref, state::Cards::Card const& source_card,
					state::CardRef target_ref, state::Cards::Card const& target_card, int amount);

			private:
				void DoDamage(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card, int amount);
				void DoHeal(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card, int amount);
			};
		}
	}
}