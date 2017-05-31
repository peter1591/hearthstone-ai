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

	template <typename HandleClass, typename EnchantmentType, typename EmitPolicy, FlowControl::aura::UpdatePolicy UpdatePolicy>
	class AuraHelper
	{
	public:
		AuraHelper(state::Cards::CardData & card_data)
		{
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext&& context) {
				context.manipulate_.Aura().Add(
					FlowControl::aura::Handler(context.card_ref_)
					.SetUpdatePolicy(UpdatePolicy)
					.SetEmitPolicy(EmitPolicy::emit_policy)
					.SetEffect(FlowControl::aura::EffectHandler_Enchantments()
						.SetGetTargets(HandleClass::GetAuraTargets)
						.SetEnchantmentType<EnchantmentType>()
					));
			};
		}
	};

	template <typename HandleClass, typename EnchantmentType, typename EmitPolicy, FlowControl::aura::UpdatePolicy UpdatePolicy>
	class SingleEnchantmentAuraHelper
	{
	public:
		SingleEnchantmentAuraHelper(state::Cards::CardData & card_data)
		{
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext&& context) {
				context.manipulate_.Aura().Add(
					FlowControl::aura::Handler(context.card_ref_)
					.SetUpdatePolicy(UpdatePolicy)
					.SetEmitPolicy(EmitPolicy::emit_policy)
					.SetEffect(FlowControl::aura::EffectHandler_Enchantment()
						.SetGetTarget(HandleClass::GetAuraTarget)
						.SetEnchantmentType<EnchantmentType>()
					));
			};
		}
	};

	template <typename HandleClass, typename EmitPolicy, FlowControl::aura::UpdatePolicy UpdatePolicy>
	struct BoardFlagAuraHelper {
		BoardFlagAuraHelper(state::Cards::CardData & card_data) {
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext&& context) {
				context.manipulate_.Aura().Add(
					FlowControl::aura::Handler(context.card_ref_)
					.SetUpdatePolicy(UpdatePolicy)
					.SetEmitPolicy(EmitPolicy::emit_policy)
					.SetEffect(FlowControl::aura::EffectHandler_BoardFlag()
						.SetApplyOn(HandleClass::AuraApplyOn)
						.SetRemoveFrom(HandleClass::AuraRemoveFrom)
					));
			};
		}
	};

	template <typename HandleClass, typename EmitPolicy, FlowControl::aura::UpdatePolicy UpdatePolicy>
	struct OwnerPlayerFlagAuraHelper {
		OwnerPlayerFlagAuraHelper(state::Cards::CardData & card_data) {
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext&& context) {
				context.manipulate_.Aura().Add(
					FlowControl::aura::Handler(context.card_ref_)
					.SetUpdatePolicy(UpdatePolicy)
					.SetEmitPolicy(EmitPolicy::emit_policy)
					.SetEffect(FlowControl::aura::EffectHandler_OwnerPlayerFlag()
						.SetApplyOn(HandleClass::AuraApplyOn)
						.SetRemoveFrom(HandleClass::AuraRemoveFrom)
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
	using AdjacentBuffHelper = AuraHelper<AdjacentBuffHandleClass, EnchantmentType, EmitWhenAlive, FlowControl::aura::UpdatePolicy::kUpdateWhenMinionChanges>;


	// For enrage
	template <typename HandleClass>
	struct EnrageWrappedHandleClass {
		static auto GetAuraTarget(FlowControl::aura::contexts::AuraGetTarget context) {
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			if (card.GetDamage() == 0) {
				return; // not enraged, apply to no one
			}
			else {
				HandleClass::GetEnrageTarget(context);
			}
		}
	};

	template <typename OriginEnchantmentType>
	struct EnrageWrappedEnchantmentType : public OriginEnchantmentType {
		static constexpr FlowControl::enchantment::EnchantmentTiers aura_tier = 
			OriginEnchantmentType::normal_tier; // priority of enrage enchantment acts as normal enchantment
	};

	template <typename HandleClass, typename OriginEnchantmentType>
	using EnrageHelper = SingleEnchantmentAuraHelper<EnrageWrappedHandleClass<HandleClass>, EnrageWrappedEnchantmentType<OriginEnchantmentType>,
		EmitWhenAlive, FlowControl::aura::UpdatePolicy::kUpdateWhenEnrageChanges>;
}