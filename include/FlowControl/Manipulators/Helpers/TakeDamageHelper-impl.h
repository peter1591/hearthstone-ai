#pragma once

#include "State/State.h"

namespace FlowControl
{
	namespace Helpers { class DamageDealer; }

	namespace Manipulators
	{
		namespace Helpers
		{
			inline void TakeDamageHelper::Take(int origin_damage)
			{
				Context::OnTakeDamage context(state_, card_ref_, origin_damage);
				state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(context);

				// Hooked events might change the damage amount, and/or the damage target
				auto const& target_card = state_.mgr.Get(context.card_ref_);
				Manipulate(state_, flow_context_).Character(context.card_ref_).Internal_SetDamage().TakeDamage(context.damage_);

				flow_context_.AddDeadEntryHint(state_, context.card_ref_);
			}
		}
	}
}