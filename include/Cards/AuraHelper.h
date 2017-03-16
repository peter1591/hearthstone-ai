#pragma once

#include "state/targetor/TargetsGenerator.h"
#include "FlowControl/aura/Handler.h"
#include "Cards/MinionCardUtils.h"

namespace Cards
{
	struct EmitWhenAlive {
		static void RegisterAura(state::Cards::CardData &card_data) {
			card_data.added_to_play_zone += [](auto context) {
				context.state_.GetAuraManager().Add(context.card_ref_);
			};
		}

		template <typename Context>
		static bool ShouldEmit(Context&& context, state::CardRef card_ref) {
			state::Cards::Card const& card = context.state_.GetCardsManager().Get(card_ref);
			if (card.GetZone() != state::kCardZonePlay) return false;
			// TODO: if (MinionCardUtils::IsSilenced(card)) return false;
			if (card.GetHP() <= 0) return false;
			return true;
		}
	};

	struct EmitWhenInHand {
		// TODO: implement
	};

	// update policy
	struct UpdateAlways {
		static bool NeedUpdate(FlowControl::aura::contexts::AuraIsValid context) { return true; }
		static void AfterUpdated(FlowControl::aura::contexts::AuraGetTargets context) {}
	};
	struct UpdateWhenMinionChanged {
		static bool NeedUpdate(FlowControl::aura::contexts::AuraIsValid context) {
			if (context.state_.GetBoard().GetFirst().minions_.GetChangeId() !=
				context.aura_handler_.last_updated_change_id_first_player_minions_) return true;
			if (context.state_.GetBoard().GetSecond().minions_.GetChangeId() !=
				context.aura_handler_.last_updated_change_id_second_player_minions_) return true;
			return false;
		}
		static void AfterUpdated(FlowControl::aura::contexts::AuraGetTargets context) {
			context.aura_handler_.last_updated_change_id_first_player_minions_ =
				context.state_.GetBoard().GetFirst().minions_.GetChangeId();
			context.aura_handler_.last_updated_change_id_second_player_minions_ =
				context.state_.GetBoard().GetSecond().minions_.GetChangeId();
		}
	};

	template <typename HandleClass, typename EnchantmentType, typename EmitPolicy, typename UpdatePolicy>
	class AuraHelper
	{
	public:
		AuraHelper(state::Cards::CardData & card_data) : card_data_(card_data)
		{
			card_data_.aura_handler.SetCallback_IsValid([](auto context) {
				if (!EmitPolicy::ShouldEmit(context, context.card_ref_)) return false;
				context.need_update_ = UpdatePolicy::NeedUpdate(context);
				return true;
			});

			card_data_.aura_handler.SetCallback_GetTargets([](auto context) {
				UpdatePolicy::AfterUpdated(context);
				return HandleClass::GetAuraTargets(context).GetInfo();
			});

			card_data_.aura_handler.SetCallback_ApplyOn([](auto context) {
				context.enchant_id_ = MinionCardUtils::Manipulate(context).Card(context.target_).Enchant().Add(EnchantmentType());
			});
			card_data_.aura_handler.SetCallback_RemoveFrom([](auto context) {
				MinionCardUtils::Manipulate(context).Card(context.target_).Enchant().Remove<EnchantmentType>(context.enchant_id_);
			});

			EmitPolicy::RegisterAura(card_data_);
		}

	private:
		state::Cards::CardData & card_data_;
	};
}