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
					state::CardRef source_ref, state::CardRef target_ref, int amount);

				void ConductDamage(int amount);

			private:
				void DoDamage(state::CardRef card_ref, int amount);
				void DoHeal(state::CardRef card_ref, int amount);

			private:
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef source_ref_;
				state::CardRef target_ref_;
				int amount_;
			};
		}
	}
}