#pragma once

#include <assert.h>
#include "FlowControl/Dispatchers/Auras.h"
#include "FlowControl/Manipulators/Helpers/AuraHelper.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			void AuraHelper::Update()
			{
				state::Cards::AuraAuxData & data = card_.GetMutableAuraAuxDataGetter().Get();

				std::unordered_set<state::CardRef> new_targets;
				Dispatchers::Auras::GetEligibles(card_.GetCardId(), state_, card_ref_, card_, new_targets);

				for (auto it = data.applied_enchantments.begin(), it2 = data.applied_enchantments.end(); it != it2;)
				{
					auto new_target_it = new_targets.find(it->first);
					if (new_target_it != new_targets.end()) {
						new_targets.erase(new_target_it);
						++it;
					}
					else {
						Manipulate(state_, flow_context_).Minion(it->first).Enchant().RemoveAuraEnchantment(it->second);
						it = data.applied_enchantments.erase(it);
					}
				}

				for (auto new_target : new_targets) {
					assert(data.applied_enchantments.find(new_target) == data.applied_enchantments.end());

					state::Cards::Enchantments::ApplyFunctor enchantment_functor;
					Dispatchers::Auras::CreateEnchantmentFor(card_.GetCardId(), state_, new_target, enchantment_functor);

					auto enchant_identifier = Manipulate(state_, flow_context_).Minion(new_target).Enchant()
						.AddAuraEnchant(enchantment_functor);
					data.applied_enchantments.insert(std::make_pair(new_target, std::move(enchant_identifier)));
				}
			}

		}
	}
}