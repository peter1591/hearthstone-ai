#pragma once

#include <unordered_map>

namespace FlowControl {
	class FlowContext;

	namespace aura {
		namespace contexts {
			struct AuraGetTargets;
			struct AuraApplyOn;
		}

		class EffectHandler_Enchantments {
		public:
			typedef void FuncGetTargets(contexts::AuraGetTargets const& context);
			typedef enchantment::TieredEnchantments::IdentifierType FuncApplyOn(contexts::AuraApplyOn const& context);

			EffectHandler_Enchantments() : get_targets(nullptr), apply_on(nullptr), applied_enchantments() {}

			EffectHandler_Enchantments& SetGetTargets(FuncGetTargets* callback) { get_targets = callback; return *this; }

			template <typename EnchantmentType> EffectHandler_Enchantments& SetEnchantmentType();

			EffectHandler_Enchantments& SetApplyOn(FuncApplyOn* callback) { apply_on = callback; return *this; }

			bool IsCallbackSet_GetTargets() const { return get_targets != nullptr; }
			bool IsCallbackSet_ApplyOn() const { return apply_on != nullptr; }

			bool NoAppliedEnchantment() const { return applied_enchantments.empty(); }
			void AfterCopied() { applied_enchantments.clear(); }

			void Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, bool aura_valid);

		private:
			FuncGetTargets * get_targets;
			FuncApplyOn * apply_on;

			std::unordered_map<state::CardRef, enchantment::TieredEnchantments::IdentifierType> applied_enchantments;
		};
	}
}