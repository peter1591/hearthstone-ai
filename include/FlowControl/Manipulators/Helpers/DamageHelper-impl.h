#pragma once

#include "FlowControl/Manipulators/Helpers/DamageHelper.h"
#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline void DamageHelper::CalculateAmount()
			{
				flow_context_.damage_ = amount_;
			}

			inline void DamageHelper::ConductDamage()
			{
				state::CardRef final_target = target_ref_;
				state::Cards::Card const* final_target_card = &target_card_;
				state_.TriggerEvent<state::Events::EventTypes::PrepareHealDamageTarget>(
					state::Events::EventTypes::PrepareHealDamageTarget::Context{ state_, flow_context_, source_ref_, source_card_, &final_target, final_target_card });

				if (!final_target.IsValid()) return;

				if (flow_context_.damage_ > 0) {
					if (final_target_card->GetRawData().enchanted_states.shielded) {
						Manipulate(state_, flow_context_).Character(final_target).Shield(false);
						return;
					}
					DoDamage(state_, flow_context_, final_target, *final_target_card, flow_context_.damage_);
				}
				else if (flow_context_.damage_ < 0) {
					DoHeal(state_, flow_context_, final_target, *final_target_card, -flow_context_.damage_);
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