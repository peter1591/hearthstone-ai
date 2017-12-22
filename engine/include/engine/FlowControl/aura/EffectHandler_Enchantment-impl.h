#pragma once

#include "engine/FlowControl/aura/EffectHandler_Enchantment.h"

namespace engine {
	namespace FlowControl
	{
		namespace aura
		{
			template <typename EnchantmentType>
			inline EffectHandler_Enchantment& EffectHandler_Enchantment::SetEnchantmentType() {
				apply_on = [](contexts::AuraApplyOn const& context) {
					return context.manipulate_.Card(context.target_).Enchant().AddAuraEnchantment(EnchantmentType());
				};
				return *this;
			}

			inline void EffectHandler_Enchantment::Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, bool aura_valid)
			{
				assert(get_target);
				assert(apply_on);

				state::CardRef new_target;
				if (aura_valid) (*get_target)({ Manipulate(state, flow_context), card_ref, new_target });

				// check if enchantment vanished
				if (applied_enchantment.first.IsValid()) {
					if (!state.GetCard(applied_enchantment.first).GetRawData().enchantment_handler.Exists(applied_enchantment.second)) {
						applied_enchantment.first.Invalidate();
					}
				}

				if (new_target == applied_enchantment.first) return; // already applied

				if (applied_enchantment.first.IsValid()) {
					Manipulate(state, flow_context).Card(applied_enchantment.first)
						.Enchant().Remove(applied_enchantment.second);
					applied_enchantment.first.Invalidate();
				}

				if (new_target.IsValid()) {
					assert(!applied_enchantment.first.IsValid());
					applied_enchantment.first = new_target;
					applied_enchantment.second = (*apply_on)({ Manipulate(state, flow_context), card_ref, new_target });
				}
			}
		}
	}
}