#pragma once

#include "state/State.h"
#include "FlowControl/Manipulators/Helpers/HealHelper.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline HealHelper::HealHelper(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, int amount)
			{
				state::Events::EventTypes::OnHeal::Context context{ state, card, amount };
				state.TriggerEvent<state::Events::EventTypes::OnHeal>(card_ref, context);
				state.TriggerCategorizedEvent<state::Events::EventTypes::OnHeal>(card_ref, context);

				Manipulators::CharacterManipulator(state, flow_context, card_ref, card)
					.Internal_SetDamage().Heal(amount);
			}
		}
	}
}