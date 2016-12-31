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
				state::Events::EventTypes::OnTakeDamage::Context context{ state, flow_context, card_ref, amount };
				state.event_mgr.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(context);

				// Hooked events might change the damage amount, and/or the damage target
				// So we should refer to the info in context
				auto const& target_card = state.mgr.Get(context.card_ref_);
				Manipulate(state, flow_context).Card(context.card_ref_).Internal_SetDamage().TakeDamage(context.damage_);

				flow_context.AddDeadEntryHint(state, context.card_ref_);
			}
		}
	}
}