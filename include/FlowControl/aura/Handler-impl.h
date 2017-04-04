#pragma once

#include <unordered_set>
#include "FlowControl/aura/Handler.h"

#include "state/State.h"
#include "state/targetor/TargetsGenerator.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/aura/EffectHandler_Enchantments-impl.h"

namespace FlowControl
{
	namespace aura
	{
		inline bool Handler::Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
		{
			bool aura_valid = IsValid(state, flow_context, card_ref);
			if (aura_valid) {
				if (!NeedUpdate(state, flow_context, card_ref)) return aura_valid;
			}

			std::visit([&](auto& item) {
				item.Update(state, flow_context, card_ref, aura_valid);
			}, effect_);

			AfterUpdated(state, flow_context, card_ref);

			return aura_valid;
		}

		inline bool Handler::NeedUpdate(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
		{
			if (update_policy_ == kUpdateAlways) {
				return true;
			}
			else if (update_policy_ == kUpdateWhenMinionChanges) {
				if (Manipulate(state, flow_context).Board().FirstPlayer().minions_.GetChangeId() !=
					last_updated_change_id_first_player_minions_) return true;
				if (Manipulate(state, flow_context).Board().SecondPlayer().minions_.GetChangeId() !=
					last_updated_change_id_second_player_minions_) return true;
				return false;
			}
			else if (update_policy_ == kUpdateWhenEnrageChanges) {
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
			if (update_policy_ == kUpdateAlways) return;
			else if (update_policy_ == kUpdateWhenMinionChanges) {
				last_updated_change_id_first_player_minions_ =
					Manipulate(state, flow_context).Board().FirstPlayer().minions_.GetChangeId();
				last_updated_change_id_second_player_minions_ =
					Manipulate(state, flow_context).Board().SecondPlayer().minions_.GetChangeId();
			}
			else if (update_policy_ == kUpdateWhenEnrageChanges) {
				first_time_update_ = false;
				state::Cards::Card const& card = Manipulate(state, flow_context).GetCard(card_ref);
				bool now_undamaged = (card.GetDamage() == 0);
				last_updated_undamaged_ = now_undamaged;
			}
			else {
				assert(false);
			}
		}
		
		inline bool Handler::IsValid(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
		{
			if (state.GetCard(card_ref).IsSilenced()) return false;

			if (emit_policy_ == kEmitWhenAlive) {
				state::Cards::Card const& card = state.GetCard(card_ref);
				if (card.GetZone() != state::kCardZonePlay) return false;
				if (card.GetHP() <= 0) return false;
				return true;
			}
			else {
				assert(false);
				return false;
			}
		}
	}
}