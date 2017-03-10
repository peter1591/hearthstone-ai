#pragma once

#include "FlowControl/Manipulators/Helpers/DamageHelper.h"
#include "State/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline DamageHelper::DamageHelper(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, int amount)
			{
				state::Events::EventTypes::OnTakeDamage::Context context{ state, flow_context, amount };
				state.event_mgr.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(card_ref, context);
				state.event_mgr.TriggerCategorizedEvent<state::Events::EventTypes::OnTakeDamage>(card_ref, context);

				// Hooked events might change the damage amount, and/or the damage target
				// So we should refer to the info in context
				auto const& target_card = state.mgr.Get(card_ref);
				Manipulate(state, flow_context).Card(card_ref).Internal_SetDamage().TakeDamage(context.damage_);

				flow_context.AddDeadEntryHint(state, card_ref);
			}
		}
	}
}