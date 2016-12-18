#pragma once

#include "State/State.h"
#include "FlowControl/Manipulators/Helpers/HealHelper.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			HealHelper::HealHelper(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, int amount)
			{
				state::Events::EventTypes::OnHeal::Context context{ state, card_ref, card, amount };
				state.event_mgr.TriggerEvent<state::Events::EventTypes::OnHeal>(context);

				Manipulators::CharacterManipulator(state, flow_context, card_ref, card)
					.Internal_SetDamage().Heal(amount);
			}
		}
	}
}