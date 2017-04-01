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
			assert(is_valid != nullptr);
			assert(get_targets);
			assert(apply_on);

			std::unordered_set<state::CardRef> new_targets;
			bool need_update = true;
			bool aura_valid = true;
			GetNewTargets(state, flow_context, card_ref, card, &aura_valid, &need_update, &new_targets);
			if (aura_valid && !need_update) return true;

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
					(*apply_on)({ Manipulate(state, flow_context), card_ref, card, new_target })
					));
			}

			return aura_valid;
		}

		inline void Handler::GetNewTargets(
			state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card,
			bool* aura_valid, bool* need_update, std::unordered_set<state::CardRef>* new_targets)
		{
			if (card.IsSilenced()) {
				*aura_valid = false;
				return;
			}

			*aura_valid = (*is_valid)({ Manipulate(state, flow_context), card_ref, card, *this, *need_update });
			if (*aura_valid && *need_update) {
				(*get_targets)({ Manipulate(state, flow_context), card_ref, card, *this, *new_targets });
			}
		}
	}
}