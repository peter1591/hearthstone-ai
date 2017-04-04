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
		static constexpr FlowControl::aura::EmitPolicy emit_policy = FlowControl::aura::kEmitWhenAlive;
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
				context.manipulate_.Aura().Add(context.card_ref_,
					FlowControl::aura::Handler()
					.SetUpdatePolicy(UpdatePolicy::update_policy)
					.SetEmitPolicy(EmitPolicy::emit_policy)
					.SetEffect(
						FlowControl::aura::EffectHandler_Enchantments()
						.SetGetTargets(HandleClass::GetAuraTargets)
						.SetEnchantmentType<EnchantmentType>()
					));
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