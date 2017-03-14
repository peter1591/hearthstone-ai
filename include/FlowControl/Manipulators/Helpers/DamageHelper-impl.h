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
				// TODO: add a new event type: OnTakeDamage_DecideTarget
				// this might change the card_ref

				auto const& target_card = state.GetCardsManager().Get(card_ref);
				state::Events::EventTypes::OnTakeDamage::Context context{ state, flow_context, target_card, amount };
				state.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(card_ref, context);
				state.TriggerCategorizedEvent<state::Events::EventTypes::OnTakeDamage>(card_ref, context);

				if (context.damage_ <= 0) return;
				Manipulate(state, flow_context).Card(card_ref).Internal_SetDamage().TakeDamage(context.damage_);
				flow_context.AddDeadEntryHint(state, card_ref);
			}
		}
	}
}