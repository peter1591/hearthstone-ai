#pragma once

#include "state/targetor/TargetsGenerator.h"
#include "engine/FlowControl/aura/Handler.h"
#include "Cards/MinionCardUtils.h"

namespace Cards
{
	struct EmitWhenAlive {
		static auto& GetRegisterCallback(state::Cards::CardData &card_data) {
			return card_data.added_to_play_zone;
		}
		static constexpr engine::FlowControl::aura::EmitPolicy emit_policy = engine::FlowControl::aura::kEmitWhenAlive;
	};

	template <typename HandleClass, typename EnchantmentType, typename EmitPolicy, engine::FlowControl::aura::UpdatePolicy UpdatePolicy>
	class AuraHelper
	{
	public:
		AuraHelper(state::Cards::CardData & card_data)
		{
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext const& context) {
				context.state_.GetAuraManager().Add(
					engine::FlowControl::aura::Handler()
					.SetOwner(context.card_ref_)
					.SetUpdatePolicy(UpdatePolicy)
					.SetEmitPolicy(EmitPolicy::emit_policy)
					.SetEffect(engine::FlowControl::aura::EffectHandler_Enchantments()
						.SetGetTargets(HandleClass::GetAuraTargets)
						.template SetEnchantmentType<EnchantmentType>()
					));
			};
		}
	};

	template <typename HandleClass, typename EnchantmentType, typename EmitPolicy, engine::FlowControl::aura::UpdatePolicy UpdatePolicy>
	class SingleEnchantmentAuraHelper
	{
	public:
		SingleEnchantmentAuraHelper(state::Cards::CardData & card_data)
		{
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext const& context) {
				context.state_.GetAuraManager().Add(
					engine::FlowControl::aura::Handler()
					.SetOwner(context.card_ref_)
					.SetUpdatePolicy(UpdatePolicy)
					.SetEmitPolicy(EmitPolicy::emit_policy)
					.SetEffect(engine::FlowControl::aura::EffectHandler_Enchantment()
						.SetGetTarget(HandleClass::GetAuraTarget)
						.template SetEnchantmentType<EnchantmentType>()
					));
			};
		}
	};

	template <typename HandleClass, typename EmitPolicy, engine::FlowControl::aura::UpdatePolicy UpdatePolicy>
	struct BoardFlagAuraHelper {
		BoardFlagAuraHelper(state::Cards::CardData & card_data) {
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext const& context) {
				context.state_.GetAuraManager().Add(
					engine::FlowControl::aura::Handler()
					.SetOwner(context.card_ref_)
					.SetUpdatePolicy(UpdatePolicy)
					.SetEmitPolicy(EmitPolicy::emit_policy)
					.SetEffect(engine::FlowControl::aura::EffectHandler_BoardFlag()
						.SetApplyOn(HandleClass::AuraApplyOn)
						.SetRemoveFrom(HandleClass::AuraRemoveFrom)
					));
			};
		}
	};

	template <typename HandleClass, typename EmitPolicy, engine::FlowControl::aura::UpdatePolicy UpdatePolicy>
	struct OwnerPlayerFlagAuraHelper {
		OwnerPlayerFlagAuraHelper(state::Cards::CardData & card_data) {
			EmitPolicy::GetRegisterCallback(card_data) += [](state::Cards::ZoneChangedContext const& context) {
				context.state_.GetAuraManager().Add(
					engine::FlowControl::aura::Handler()
					.SetOwner(context.card_ref_)
					.SetUpdatePolicy(UpdatePolicy)
					.SetEmitPolicy(EmitPolicy::emit_policy)
					.SetEffect(engine::FlowControl::aura::EffectHandler_OwnerPlayerFlag()
						.SetApplyOn(HandleClass::AuraApplyOn)
						.SetRemoveFrom(HandleClass::AuraRemoveFrom)
					));
			};
		}
	};

	// for adjacent buff
	struct AdjacentBuffHandleClass {
		static auto GetAuraTargets(engine::FlowControl::aura::contexts::AuraGetTargets const& context) {
			state::Cards::Card const& card = context.manipulate_.GetCard(context.card_ref_);
			assert(card.GetZone() == state::kCardZonePlay);
			assert(card.GetCardType() == state::kCardTypeMinion);
			int zone_pos = card.GetZonePosition();
			auto & minions = context.manipulate_.Board().Player(card.GetPlayerIdentifier()).minions_;
			assert(minions.Get(zone_pos) == context.card_ref_);
			if (zone_pos > 0) context.new_targets.push_back(minions.Get(zone_pos - 1));
			if (zone_pos < ((int)minions.Size() - 1)) context.new_targets.push_back(minions.Get(zone_pos + 1));
		}
	};
	template <typename HandleClass, typename EnchantmentType>
	using AdjacentBuffHelper = AuraHelper<AdjacentBuffHandleClass, EnchantmentType, EmitWhenAlive, engine::FlowControl::aura::UpdatePolicy::kUpdateWhenMinionChanges>;


	// For enrage
	template <typename HandleClass>
	struct EnrageWrappedHandleClass {
		static auto GetAuraTarget(engine::FlowControl::aura::contexts::AuraGetTarget const& context) {
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
		static constexpr engine::FlowControl::enchantment::EnchantmentTiers aura_tier =
			OriginEnchantmentType::normal_tier; // priority of enrage enchantment acts as normal enchantment
	};

	template <typename HandleClass, typename OriginEnchantmentType>
	using EnrageHelper = SingleEnchantmentAuraHelper<EnrageWrappedHandleClass<HandleClass>, EnrageWrappedEnchantmentType<OriginEnchantmentType>,
		EmitWhenAlive, engine::FlowControl::aura::UpdatePolicy::kUpdateWhenEnrageChanges>;
}