#pragma once

#include "FlowControl/aura/EffectHandler_Enchantments.h"

namespace FlowControl
{
	namespace aura
	{
		template <typename EnchantmentType>
		inline EffectHandler_Enchantments& EffectHandler_Enchantments::SetEnchantmentType() {
			apply_on = [](contexts::AuraApplyOn context) {
				return context.manipulate_.Card(context.target_).Enchant().AddAuraEnchantment(EnchantmentType());
			};
			return *this;
		}

		inline void EffectHandler_Enchantments::Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, bool aura_valid)
		{
			assert(get_targets);
			assert(apply_on);

			std::unordered_set<state::CardRef> new_targets;
			if (aura_valid) (*get_targets)({ Manipulate(state, flow_context), card_ref, new_targets });

			for (auto it = applied_enchantments.begin(), it2 = applied_enchantments.end(); it != it2;)
			{
				if (!state.GetCard(it->first).GetRawData().enchantment_handler.Exists(it->second)) {
					// enchantment vanished
					it = applied_enchantments.erase(it);
					continue;
				}

				auto new_target_it = new_targets.find(it->first);
				if (new_target_it != new_targets.end()) {
					// already applied
					new_targets.erase(new_target_it);
					++it;
				}
				else {
					// enchantments should be removed
					Manipulate(state, flow_context).Card(it->first).Enchant().Remove(it->second);
					it = applied_enchantments.erase(it);
				}
			}

			for (auto new_target : new_targets) {
				// enchantments should be applied
				assert(applied_enchantments.find(new_target) == applied_enchantments.end());

				applied_enchantments.insert(std::make_pair(new_target,
					(*apply_on)({ Manipulate(state, flow_context), card_ref, new_target })
				));
			}
		}
	}
}