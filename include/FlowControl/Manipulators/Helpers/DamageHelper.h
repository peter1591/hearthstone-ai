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
					state::CardRef source_ref, state::CardRef target_ref, int amount)
					: state_(state), flow_context_(flow_context),
					source_ref_(source_ref), target_ref_(target_ref), amount_(amount)
				{
					assert(state.GetCard(target_ref_).GetCardType() == state::kCardTypeMinion ||
						state.GetCard(target_ref_).GetCardType() == state::kCardTypeHero ||
						state.GetCard(target_ref_).GetCardType() == state::kCardTypeWeapon);
				}

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