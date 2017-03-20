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
					state::CardRef target_ref, state::Cards::Card const& target_card, int amount)
					: state_(state), flow_context_(flow_context),
					source_ref_(source_ref), source_card_(source_card),
					target_ref_(target_ref), target_card_(target_card), amount_(amount)
				{
					assert(target_card.GetCardType() == state::kCardTypeMinion ||
						target_card.GetCardType() == state::kCardTypeHero ||
						target_card.GetCardType() == state::kCardTypeWeapon);
				}

				void ConductDamage(int amount);

			private:
				void DoDamage(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card, int amount);
				void DoHeal(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card, int amount);

			private:
				state::State & state_;
				FlowControl::FlowContext & flow_context_;
				state::CardRef source_ref_;
				state::Cards::Card const& source_card_;
				state::CardRef target_ref_;
				state::Cards::Card const& target_card_;
				int amount_;
			};
		}
	}
}