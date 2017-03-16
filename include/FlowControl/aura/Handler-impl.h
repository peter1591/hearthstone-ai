#pragma once

#include <unordered_set>
#include "FlowControl/aura/Handler.h"

#include "state/State.h"
#include "state/targetor/TargetsGenerator.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace aura
	{
		inline bool Handler::Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card)
		{
			assert(this == &card.GetRawData().aura_handler);
			assert(is_valid != nullptr);
			assert(get_targets);
			assert(apply_on);
			assert(remove_from);

			std::unordered_set<state::CardRef> new_targets;

			bool need_update = true;
			bool aura_valid = (*is_valid)({ state, flow_context, card_ref, card, *this, need_update });
			if (aura_valid) {
				if (!need_update) aura_valid;
				(*get_targets)({ state, flow_context, card_ref, card, *this})
					.Fill(state, new_targets);
			}

			for (auto it = applied_enchantments.begin(), it2 = applied_enchantments.end(); it != it2;)
			{
				auto new_target_it = new_targets.find(it->first);
				if (new_target_it != new_targets.end()) {
					// already applied
					new_targets.erase(new_target_it);
					++it;
				}
				else {
					// enchantments should be removed
					(*remove_from)({ state, flow_context, card_ref, card, it->first, it->second });
					it = applied_enchantments.erase(it);
				}
			}

			for (auto new_target : new_targets) {
				// enchantments should be applied
				assert(applied_enchantments.find(new_target) == applied_enchantments.end());

				FlowControl::enchantment::Enchantments::ContainerType::Identifier enchant_id;
				(*apply_on)({ state, flow_context, card_ref, card, new_target, enchant_id });
				applied_enchantments.insert(std::make_pair(new_target, std::move(enchant_id)));
			}

			return aura_valid;
		}
	}
}