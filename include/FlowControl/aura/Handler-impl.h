#pragma once

#include <unordered_set>
#include "FlowControl/aura/Handler.h"

#include "state/State.h"
#include "state/targetor/TargetsGenerator.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/aura/EffectHandler_Enchantment-impl.h"
#include "FlowControl/aura/EffectHandler_Enchantments-impl.h"
#include "FlowControl/aura/EffectHandler_BoardFlag-impl.h"
#include "FlowControl/aura/EffectHandler_OwnerPlayerFlag-impl.h"

namespace FlowControl
{
	namespace aura
	{
		inline bool Handler::Update(state::State & state, FlowControl::FlowContext & flow_context)
		{
			bool aura_valid = IsValid(state, flow_context);
			if (aura_valid) {
				if (!NeedUpdate(state, flow_context)) return aura_valid;
			}

			std::visit([&](auto& item) {
				item.Update(state, flow_context, owner_ref_, aura_valid);
			}, effect_);

			AfterUpdated(state, flow_context);

			return aura_valid;
		}

		inline bool Handler::NeedUpdate(state::State & state, FlowControl::FlowContext & flow_context)
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
				state::Cards::Card const& card = Manipulate(state, flow_context).GetCard(owner_ref_);
				bool now_undamaged = (card.GetDamage() == 0);
				return (last_updated_undamaged_ != now_undamaged);
			}
			else if (update_policy_ == kUpdateOwnerChanges) {
				if (!last_updated_owner_.IsValid()) return true;
				return state.GetCard(owner_ref_).GetPlayerIdentifier() != last_updated_owner_;
			}
			else if (update_policy_ == kUpdateOnlyFirstTime) {
				return first_time_update_;
			}
			else {
				assert(false);
				return true;
			}
		}

		inline void Handler::AfterUpdated(state::State & state, FlowControl::FlowContext & flow_context)
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
				state::Cards::Card const& card = Manipulate(state, flow_context).GetCard(owner_ref_);
				bool now_undamaged = (card.GetDamage() == 0);
				last_updated_undamaged_ = now_undamaged;
			}
			else if (update_policy_ == kUpdateOwnerChanges) {
				last_updated_owner_ = state.GetCard(owner_ref_).GetPlayerIdentifier();
			}
			else if (update_policy_ == kUpdateOnlyFirstTime) {
				first_time_update_ = false;
			}
			else {
				assert(false);
			}
		}
		
		inline bool Handler::IsValid(state::State & state, FlowControl::FlowContext & flow_context)
		{
			if (state.GetCard(owner_ref_).IsSilenced()) return false;

			if (emit_policy_ == kEmitWhenAlive) {
				state::Cards::Card const& card = state.GetCard(owner_ref_);
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