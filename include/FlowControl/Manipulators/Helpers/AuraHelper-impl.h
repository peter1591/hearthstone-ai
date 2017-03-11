#pragma once
#include "FlowControl/Manipulators/Helpers/AuraHelper.h"

#include <assert.h>
#include "FlowControl/Contexts.h"
#include "FlowControl/Manipulate.h"
#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline void AuraHelper::Update()
			{
				if (card_.GetRawData().aura_handler.get_targets == nullptr) return; // no aura attached

				assert(card_.GetRawData().aura_handler.apply_on);
				assert(card_.GetRawData().aura_handler.remove_from);

				state::Cards::AuraAuxData & data = card_.GetMutableAuraAuxDataGetter().Get();

				std::unordered_set<state::CardRef> new_targets;
				(*card_.GetRawData().aura_handler.get_targets)
					({ state_, flow_context_, card_ref_, card_, data, new_targets });

				for (auto it = data.applied_enchantments.begin(), it2 = data.applied_enchantments.end(); it != it2;)
				{
					auto new_target_it = new_targets.find(it->first);
					if (new_target_it != new_targets.end()) {
						// already applied
						new_targets.erase(new_target_it);
						++it;
					}
					else {
						// enchantments should be removed
						(*card_.GetRawData().aura_handler.remove_from)
							({ state_, flow_context_, card_ref_, card_, it->first, it->second });
						it = data.applied_enchantments.erase(it);
					}
				}

				for (auto new_target : new_targets) {
					// enchantments should be applied
					assert(data.applied_enchantments.find(new_target) == data.applied_enchantments.end());

					state::Cards::Enchantments::ContainerType::Identifier enchant_id;
					(*card_.GetRawData().aura_handler.apply_on)
						({ state_, flow_context_, card_ref_, card_, new_target, enchant_id });
					data.applied_enchantments.insert(std::make_pair(new_target, std::move(enchant_id)));
				}
			}
		}
	}
}