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
						Manipulate(state_, flow_context_).OnBoardCharacter(final_target).Shield(false);
						return;
					}
					if (state_.GetCard(final_target).GetImmune()) {
						return;
					}

					DoDamage(final_target, amount);

					bool is_poisonous = state_.GetCardBoolAttributeConsiderWeapon(source_ref_, [](state::Cards::Card const& card) {
						return card.IsPoisonous();
					});
					if (is_poisonous) {
						if (state_.GetCard(target_ref_).GetCardType() == state::kCardTypeMinion) {
							Manipulate(state_, flow_context_).OnBoardMinion(target_ref_).Destroy();
						}
					}

					bool is_freeze_attack = state_.GetCardBoolAttributeConsiderWeapon(source_ref_, [](state::Cards::Card const& card) {
						return card.IsFreezeAttack();
					});
					if (is_freeze_attack) {
						Manipulate(state_, flow_context_).OnBoardCharacter(target_ref_).Freeze(true);
					}
				}
				else if (amount < 0) {
					DoHeal(final_target, -amount);
				}
				else { // if (final_amount == 0)
					return;
				}
			}

			inline void DamageHelper::DoDamage(state::CardRef card_ref, int amount)
			{
				assert(amount > 0);
				state_.TriggerEvent<state::Events::EventTypes::OnTakeDamage>(
					state::Events::EventTypes::OnTakeDamage::Context{ Manipulate(state_, flow_context_), card_ref, &amount });
				state_.TriggerCategorizedEvent<state::Events::EventTypes::CategorizedOnTakeDamage>(card_ref,
					state::Events::EventTypes::CategorizedOnTakeDamage::Context{ Manipulate(state_, flow_context_), &amount });

				if (amount > 0) {
					int real_damage = 0;
					Manipulate(state_, flow_context_).Card(card_ref).Internal_SetDamage().TakeDamage(amount, &real_damage);
					if (state_.GetCard(card_ref).GetHP() <= 0) flow_context_.AddDeadEntryHint(state_, card_ref);

					state_.TriggerEvent<state::Events::EventTypes::AfterTakenDamage>(
						state::Events::EventTypes::AfterTakenDamage::Context{ Manipulate(state_, flow_context_), card_ref, amount, real_damage });
				}
			}

			inline void DamageHelper::DoHeal(state::CardRef card_ref, int amount)
			{
				assert(amount > 0);

				int current_damage = state_.GetCard(card_ref).GetDamage();
				assert(current_damage >= 0);
				if (current_damage == 0) return;

				if (amount > current_damage) amount = current_damage;

				state_.TriggerEvent<state::Events::EventTypes::OnHeal>(
					state::Events::EventTypes::OnHeal::Context{ Manipulate(state_, flow_context_), card_ref, amount });

				Manipulate(state_, flow_context_).Card(card_ref).Internal_SetDamage().Heal(amount);
			}
		}
	}
}