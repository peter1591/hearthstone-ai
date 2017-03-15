#pragma once

#include "FlowControl/Manipulators/Helpers/DamageHelper.h"
#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline DamageHelper::DamageHelper(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, int amount)
			{
				// TODO: add a new event type: PreTakeDamage
				// this might change the card_ref, and the damage amount
				state::CardRef new_target = card_ref; // TODO
				int new_amount = amount; // TODO
				if (new_amount <= 0) return;

				DoDamage(state, flow_context, new_target, state.GetCard(new_target), new_amount);
			}

			inline void DamageHelper::DoDamage(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card, int amount)
			{
				auto const& target_card = state.GetCardsManager().Get(card_ref);
				state.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(card_ref,
					state::Events::EventTypes::OnTakeDamage::Context{ state, flow_context, target_card, amount });
				state.TriggerCategorizedEvent<state::Events::EventTypes::OnTakeDamage>(card_ref,
					state::Events::EventTypes::OnTakeDamage::Context{ state, flow_context, target_card, amount });

				Manipulate(state, flow_context).Card(card_ref).Internal_SetDamage().TakeDamage(amount);
				flow_context.AddDeadEntryHint(state, card_ref);
			}
		}
	}
}