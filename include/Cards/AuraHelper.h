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
		static constexpr FlowControl::aura::UpdatePolicy update_policy = FlowControl::aura::kUpdateAlways;
	};
	struct UpdateWhenMinionChanged {
		static constexpr FlowControl::aura::UpdatePolicy update_policy = FlowControl::aura::kUpdateWhenMinionChanges;
	};
	struct UpdateWhenEnrageChanged {
		static constexpr FlowControl::aura::UpdatePolicy update_policy = FlowControl::aura::kUpdateWhenEnrageChanges;
	};

	template <typename HandleClass, typename EnchantmentType, typename EmitPolicy, typename UpdatePolicy>
	class AuraHelper
	{
	public:
		AuraHelper(state::Cards::CardData & card_data)
		{
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext&& context) {
				FlowControl::aura::Handler handler;

				handler.SetUpdatePolicy(UpdatePolicy::update_policy);

				assert(!handler.IsCallbackSet_IsValid());
				handler.SetCallback_IsValid([](auto context) {
					if (!EmitPolicy::ShouldEmit(context, context.card_ref_)) return false;
					return true;
				});

				assert(!handler.IsCallbackSet_GetTargets());
				handler.SetCallback_GetTargets([](auto context) {
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

	// for adjacent buff
	struct AdjacentBuffHandleClass {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			assert(card.GetZone() == state::kCardZonePlay);
			assert(card.GetCardType() == state::kCardTypeMinion);
			int zone_pos = card.GetZonePosition();
			auto & minions = context.manipulate_.Board().Player(card.GetPlayerIdentifier()).minions_;
			assert(minions.Get(zone_pos) == context.card_ref_);
			if (zone_pos > 0) context.new_targets.insert(minions.Get(zone_pos - 1));
			if (zone_pos < (minions.Size() - 1)) context.new_targets.insert(minions.Get(zone_pos + 1));
		}
	};
	template <typename HandleClass, typename EnchantmentType>
	using AdjacentBuffHelper = AuraHelper<AdjacentBuffHandleClass, EnchantmentType, EmitWhenAlive, UpdateWhenMinionChanged>;


	// For enrage
	template <typename HandleClass>
	struct EnrageWrappedHandleClass {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			if (card.GetDamage() == 0) {
				return; // not enraged, apply to no one
			}
			else {
				HandleClass::GetEnrageTargets(context);
			}
		}
	};

	template <typename OriginEnchantmentType>
	struct EnrageWrappedEnchantmentType : public OriginEnchantmentType {
		static constexpr FlowControl::enchantment::EnchantmentTiers aura_tier = 
			OriginEnchantmentType::normal_tier; // priority of enrage enchantment acts as normal enchantment
	};

	template <typename HandleClass, typename OriginEnchantmentType>
	using EnrageHelper = AuraHelper<EnrageWrappedHandleClass<HandleClass>, EnrageWrappedEnchantmentType<OriginEnchantmentType>,
		EmitWhenAlive, UpdateWhenEnrageChanged>;
}