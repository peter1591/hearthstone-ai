#pragma once

#include "FlowControl/Manipulators/Helpers/DamageHelper.h"
#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline void DamageHelper::ConductDamage(int amount)
			{
				state::CardRef final_target = target_ref_;
				state_.TriggerEvent<state::Events::EventTypes::PrepareHealDamageTarget>(
					state::Events::EventTypes::PrepareHealDamageTarget::Context{ Manipulate(state_, flow_context_), source_ref_, &final_target });

				if (!final_target.IsValid()) return;

				if (amount > 0) {
					if (state_.GetCard(final_target).HasShield()) {
						Manipulate(state_, flow_context_).Character(final_target).Shield(false);
						return;
					}
					DoDamage(state_, flow_context_, final_target, amount);

					if (state_.GetCard(source_ref_).IsPoisonous()) {
						if (state_.GetCard(target_ref_).GetCardType() == state::kCardTypeMinion) {
							Manipulate(state_, flow_context_).Minion(target_ref_).Destroy();
						}
					}
				}
				else if (amount < 0) {
					DoHeal(state_, flow_context_, final_target, -amount);
				}
				else { // if (final_amount == 0)
					return;
				}
			}

			inline void DamageHelper::DoDamage(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, int amount)
			{
				assert(amount > 0);
				state.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(card_ref,
					state::Events::EventTypes::OnTakeDamage::Context{ Manipulate(state_, flow_context_), amount });
				state.TriggerCategorizedEvent<state::Events::EventTypes::OnTakeDamage>(card_ref,
					state::Events::EventTypes::OnTakeDamage::Context{ Manipulate(state_, flow_context_), amount });

				int real_damage = 0;
				Manipulate(state, flow_context).Card(card_ref).Internal_SetDamage().TakeDamage(amount, &real_damage);
				if (state.GetCard(card_ref).GetHP() <= 0) flow_context.AddDeadEntryHint(state, card_ref);

				// TODO: trigger event 'AfterTakenDamage', with real damage 'real_damage'
			}

			inline void DamageHelper::DoHeal(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, int amount)
			{
				assert(amount > 0);
				state.TriggerEvent<state::Events::EventTypes::OnHeal>(
					state::Events::EventTypes::OnHeal::Context{ Manipulate(state_, flow_context_), card_ref, amount });

				Manipulate(state, flow_context).Card(card_ref).Internal_SetDamage().Heal(amount);
			}
		}
	}
}