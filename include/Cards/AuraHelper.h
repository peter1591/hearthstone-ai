#pragma once

#include "state/targetor/TargetsGenerator.h"
#include "FlowControl/aura/Handler.h"
#include "Cards/MinionCardUtils.h"

namespace Cards
{
	struct EmitWhenAlive {
		static auto& GetRegisterCallback(state::Cards::CardData &card_data) {
			return card_data.added_to_play_zone;
		}

		template <typename Context>
		static bool ShouldEmit(Context&& context, state::CardRef card_ref) {
			state::Cards::Card const& card = context.manipulate_.Board().GetCard(card_ref);
			if (card.GetZone() != state::kCardZonePlay) return false;
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
			if (context.manipulate_.Board().FirstPlayer().minions_.GetChangeId() !=
				context.aura_handler_.last_updated_change_id_first_player_minions_) return true;
			if (context.manipulate_.Board().SecondPlayer().minions_.GetChangeId() !=
				context.aura_handler_.last_updated_change_id_second_player_minions_) return true;
			return false;
		}
		static void AfterUpdated(FlowControl::aura::contexts::AuraGetTargets context) {
			context.aura_handler_.last_updated_change_id_first_player_minions_ =
				context.manipulate_.Board().FirstPlayer().minions_.GetChangeId();
			context.aura_handler_.last_updated_change_id_second_player_minions_ =
				context.manipulate_.Board().SecondPlayer().minions_.GetChangeId();
		}
	};

	template <typename HandleClass, typename EnchantmentType, typename EmitPolicy, typename UpdatePolicy>
	class AuraHelper
	{
	public:
		AuraHelper(state::Cards::CardData & card_data)
		{
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext&& context) {
				FlowControl::aura::Handler handler;

				assert(!handler.IsCallbackSet_IsValid());
				handler.SetCallback_IsValid([](auto context) {
					if (!EmitPolicy::ShouldEmit(context, context.card_ref_)) return false;
					context.need_update_ = UpdatePolicy::NeedUpdate(context);
					return true;
				});

				assert(!handler.IsCallbackSet_GetTargets());
				handler.SetCallback_GetTargets([](auto context) {
					UpdatePolicy::AfterUpdated(context);
					HandleClass::GetAuraTargets(context);
				});

				assert(!handler.IsCallbackSet_ApplyOn());
				handler.SetCallback_ApplyOn([](auto context) {
					return MinionCardUtils::Manipulate(context).Card(context.target_).Enchant().AddAuraEnchantment<EnchantmentType>();
				});
				context.manipulate_.Aura().Add(context.card_ref_, std::move(handler));
			};
		}
	};


	// For enrage
	template <typename HandleClass>
	struct EnrageWrappedHandleClass {
		template <typename Context>
		static auto GetAuraTargets(Context&& context) {
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			if (card.GetDamage() == 0) {
				return; // not enraged, apply to no one
			}
			else {
				HandleClass::GetEnrageTargets(std::forward<Context>(context));
			}
		}
	};
	struct UpdateWhenEnrageChanged {
		static bool NeedUpdate(FlowControl::aura::contexts::AuraIsValid context) {
			if (context.aura_handler_.first_time_update_) return true;
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			bool now_undamaged = (card.GetDamage() == 0);
			return (context.aura_handler_.last_updated_undamaged_ != now_undamaged);
		}
		static void AfterUpdated(FlowControl::aura::contexts::AuraGetTargets context) {
			context.aura_handler_.first_time_update_ = false;
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			bool now_undamaged = (card.GetDamage() == 0);
			context.aura_handler_.last_updated_undamaged_ = now_undamaged;
		}
	};

	template <typename HandleClass, typename EnchantmentType>
	using EnrageHelper = AuraHelper<EnrageWrappedHandleClass<HandleClass>, EnchantmentType, EmitWhenAlive, UpdateWhenEnrageChanged>;
}