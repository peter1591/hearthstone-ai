#pragma once

#include "FlowControl/Manipulators/Helpers/DamageHelper.h"
#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline DamageHelper::DamageHelper(state::State & state, FlowControl::FlowContext & flow_context,
				state::CardRef source_ref, state::Cards::Card const& source_card,
				state::CardRef target_ref, state::Cards::Card const& target_card, int amount)
			{
				state::CardRef final_target = target_ref;
				state::Cards::Card const* final_target_card = &target_card;
				int final_amount = amount;
				state.TriggerEvent<state::Events::EventTypes::PrepareHealDamage>(
					state::Events::EventTypes::PrepareHealDamage::Context{ state, flow_context, source_ref, source_card, &final_target, final_target_card, &final_amount });

				if (!final_target.IsValid()) return;

				if (final_amount > 0) {
					if (final_target_card->GetRawData().enchanted_states.shielded) {
						Manipulate(state, flow_context).Character(final_target).Shield(false);
						return;
					}
					DoDamage(state, flow_context, final_target, *final_target_card, final_amount);
				}
				else if (final_amount < 0) {
					DoHeal(state, flow_context, final_target, *final_target_card, -final_amount);
				}
				else { // if (final_amount == 0)
					return;
				}
			}

			inline void DamageHelper::DoDamage(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card, int amount)
			{
				assert(amount > 0);
				state.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(card_ref,
					state::Events::EventTypes::OnTakeDamage::Context{ state, flow_context, card, amount });
				state.TriggerCategorizedEvent<state::Events::EventTypes::OnTakeDamage>(card_ref,
					state::Events::EventTypes::OnTakeDamage::Context{ state, flow_context, card, amount });

				Manipulate(state, flow_context).Card(card_ref).Internal_SetDamage().TakeDamage(amount);
				flow_context.AddDeadEntryHint(state, card_ref);
			}

			inline void DamageHelper::DoHeal(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card, int amount)
			{
				assert(amount > 0);
				state.TriggerEvent<state::Events::EventTypes::OnHeal>(card_ref,
					state::Events::EventTypes::OnHeal::Context{ state, flow_context, card, amount });
				state.TriggerCategorizedEvent<state::Events::EventTypes::OnHeal>(card_ref,
					state::Events::EventTypes::OnHeal::Context{ state, flow_context, card, amount });

				Manipulate(state, flow_context).Card(card_ref).Internal_SetDamage().Heal(amount);
			}
		}
	}
}