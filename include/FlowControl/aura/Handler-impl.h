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
		inline bool Handler::Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
		{
			assert(is_valid != nullptr);
			assert(get_targets);
			assert(apply_on);

			std::unordered_set<state::CardRef> new_targets;
			bool need_update = true;
			bool aura_valid = true;
			GetNewTargets(state, flow_context, card_ref, &aura_valid, &need_update, &new_targets);
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
					(*apply_on)({ Manipulate(state, flow_context), card_ref, new_target })
					));
			}

			AfterUpdated(state, flow_context, card_ref);

			return aura_valid;
		}

		inline bool Handler::NeedUpdate(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
		{
			if (update_policy == kUpdateAlways) {
				return true;
			}
			else if (update_policy == kUpdateWhenMinionChanges) {
				if (Manipulate(state, flow_context).Board().FirstPlayer().minions_.GetChangeId() !=
					last_updated_change_id_first_player_minions_) return true;
				if (Manipulate(state, flow_context).Board().SecondPlayer().minions_.GetChangeId() !=
					last_updated_change_id_second_player_minions_) return true;
				return false;
			}
			else if (update_policy == kUpdateWhenEnrageChanges) {
				if (first_time_update_) return true;
				state::Cards::Card const& card = Manipulate(state, flow_context).GetCard(card_ref);
				bool now_undamaged = (card.GetDamage() == 0);
				return (last_updated_undamaged_ != now_undamaged);
			}
			else {
				assert(false);
				return true;
			}
		}

		inline void Handler::AfterUpdated(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
		{
			if (update_policy == kUpdateAlways) return;
			else if (update_policy == kUpdateWhenMinionChanges) {
				last_updated_change_id_first_player_minions_ =
					Manipulate(state, flow_context).Board().FirstPlayer().minions_.GetChangeId();
				last_updated_change_id_second_player_minions_ =
					Manipulate(state, flow_context).Board().SecondPlayer().minions_.GetChangeId();
			}
			else if (update_policy == kUpdateWhenEnrageChanges) {
				first_time_update_ = false;
				state::Cards::Card const& card = Manipulate(state, flow_context).GetCard(card_ref);
				bool now_undamaged = (card.GetDamage() == 0);
				last_updated_undamaged_ = now_undamaged;
			}
			else {
				assert(false);
			}
		}

		inline void Handler::GetNewTargets(
			state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref,
			bool* aura_valid, bool* need_update, std::unordered_set<state::CardRef>* new_targets)
		{
			if (state.GetCard(card_ref).IsSilenced()) {
				*aura_valid = false;
				return;
			}

			*aura_valid = (*is_valid)({ Manipulate(state, flow_context), card_ref, *this, *need_update });
			if (!*aura_valid) return;
			*need_update = NeedUpdate(state, flow_context, card_ref);

			if (*need_update) {
				(*get_targets)({ Manipulate(state, flow_context), card_ref, *this, *new_targets });
			}
		}
	}
}