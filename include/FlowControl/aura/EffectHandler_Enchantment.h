#pragma once

namespace FlowControl {
	class FlowContext;

	namespace aura {
		class EffectHandler_Enchantment {
		public:
			typedef void FuncGetTarget(contexts::AuraGetTarget context);
			typedef enchantment::TieredEnchantments::IdentifierType FuncApplyOn(contexts::AuraApplyOn context);

			EffectHandler_Enchantment() : get_target(nullptr), apply_on(nullptr) {}

			EffectHandler_Enchantment& SetGetTarget(FuncGetTarget* callback) { get_target = callback; return *this; }

			template <typename EnchantmentType> EffectHandler_Enchantment& SetEnchantmentType();

			EffectHandler_Enchantment& SetApplyOn(FuncApplyOn* callback) { apply_on = callback; return *this; }

			bool IsCallbackSet_GetTarget() const { return get_target != nullptr; }
			bool IsCallbackSet_ApplyOn() const { return apply_on != nullptr; }

			bool NoAppliedEnchantment() const { return !applied_enchantment.first.IsValid(); }
			void AfterCopied() { applied_enchantment.first.Invalidate(); }

			void Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, bool aura_valid);

		private:
			FuncGetTarget * get_target;
			FuncApplyOn * apply_on;

			std::pair<state::CardRef, enchantment::TieredEnchantments::IdentifierType> applied_enchantment;
		};
	}
}