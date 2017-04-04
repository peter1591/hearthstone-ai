#pragma once

#include <variant>
#include "state/targetor/Targets.h"
#include "FlowControl/enchantment/TieredEnchantments.h"
#include "FlowControl/aura/Contexts.h"
#include "FlowControl/aura/EffectHandler_Enchantments.h"

namespace FlowControl
{
	class FlowContext;

	namespace aura
	{
		enum UpdatePolicy {
			kUpdateAlways,
			kUpdateWhenMinionChanges,
			kUpdateWhenEnrageChanges
		};

		enum EmitPolicy {
			kEmitInvalid,
			kEmitWhenAlive
		};

		class Handler
		{
		public:

			Handler() :
				update_policy_(kUpdateAlways), emit_policy_(kEmitInvalid),
				first_time_update_(true),
				last_updated_change_id_first_player_minions_(-1), // ensure this is not the initial value of the actual change id
				last_updated_change_id_second_player_minions_(-1),
				last_updated_undamaged_(true)
			{}

		public:
			void SetUpdatePolicy(UpdatePolicy policy) { update_policy_ = policy; }
			void SetEmitPolicy(EmitPolicy policy) { emit_policy_ = policy; }

			template <typename EffectType>
			void SetEffect(EffectType effect) { effect_ = effect; }

		public:
			bool NoAppliedEnchantment() const {
				return std::visit([](auto& item) -> bool {
					return item.NoAppliedEnchantment();
				}, effect_);
			}
			bool Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref);

			void AfterCopied() {
				first_time_update_ = true;
				std::visit([](auto& item) {
					item.AfterCopied();
				}, effect_);
			}

		private:
			bool NeedUpdate(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref);
			void AfterUpdated(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref);

			bool IsValid(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref);

		public: // field for client code
			bool first_time_update_;
			int last_updated_change_id_first_player_minions_;
			int last_updated_change_id_second_player_minions_;
			bool last_updated_undamaged_;

		private:
			UpdatePolicy update_policy_;
			EmitPolicy emit_policy_;

			std::variant<EffectHandler_Enchantments> effect_;
		};
	}
}