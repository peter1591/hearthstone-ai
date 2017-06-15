#pragma once

#include <variant>
#include "state/targetor/Targets.h"
#include "FlowControl/enchantment/TieredEnchantments.h"
#include "FlowControl/aura/EffectHandler_Enchantment.h"
#include "FlowControl/aura/EffectHandler_Enchantments.h"
#include "FlowControl/aura/EffectHandler_BoardFlag.h"
#include "FlowControl/aura/EffectHandler_OwnerPlayerFlag.h"

namespace FlowControl
{
	class FlowContext;

	namespace aura
	{
		enum UpdatePolicy {
			kUpdateAlways,
			kUpdateWhenMinionChanges,
			kUpdateWhenEnrageChanges,
			kUpdateOwnerChanges,
			kUpdateOnlyFirstTime
		};

		enum EmitPolicy {
			kEmitInvalid,
			kEmitWhenAlive // Need SetOwner(), and it's not silenced
		};

		class Handler
		{
		public:
			Handler() :
				first_time_update_(true),
				last_updated_change_id_first_player_minions_(-1), // ensure this is not the initial value of the actual change id
				last_updated_change_id_second_player_minions_(-1),
				last_updated_undamaged_(true),
				update_policy_(kUpdateAlways), emit_policy_(kEmitInvalid)
			{}

		public:
			Handler& SetOwner(state::CardRef owner_ref) { owner_ref_ = owner_ref; return *this; }
			Handler& SetUpdatePolicy(UpdatePolicy policy) { update_policy_ = policy; return *this; }
			Handler& SetEmitPolicy(EmitPolicy policy) { emit_policy_ = policy; return *this; }

			template <typename EffectType>
			Handler& SetEffect(EffectType effect) { effect_ = effect; return *this; }

		public:
			bool NoAppliedEnchantment() const {
				return std::visit([](auto& item) -> bool {
					return item.NoAppliedEnchantment();
				}, effect_);
			}
			bool Update(state::State & state, FlowControl::FlowContext & flow_context);

			void AfterCopied() {
				first_time_update_ = true;
				std::visit([](auto& item) {
					item.AfterCopied();
				}, effect_);
			}

		private:
			bool NeedUpdate(state::State & state, FlowControl::FlowContext & flow_context);
			void AfterUpdated(state::State & state, FlowControl::FlowContext & flow_context);

			bool IsValid(state::State & state, FlowControl::FlowContext & flow_context);

		public: // field for update policy
			bool first_time_update_;
			int last_updated_change_id_first_player_minions_;
			int last_updated_change_id_second_player_minions_;
			bool last_updated_undamaged_;
			state::PlayerIdentifier last_updated_owner_;

		private:
			state::CardRef owner_ref_;
			UpdatePolicy update_policy_;
			EmitPolicy emit_policy_;

			std::variant<
				EffectHandler_Enchantment,
				EffectHandler_Enchantments,
				EffectHandler_BoardFlag,
				EffectHandler_OwnerPlayerFlag
			> effect_;
		};
	}
}