#pragma once

namespace FlowControl {
	class FlowContext;

	namespace aura {
		class EffectHandler_BoardFlag {
		public:
			typedef void FuncApplyOn(contexts::AuraApplyFlagOnBoard context);
			typedef void FuncRemoveFrom(contexts::AuraRemoveFlagFromBoard context);

			EffectHandler_BoardFlag() : apply_on(nullptr), remove_from(nullptr), applied(false) {}

			EffectHandler_BoardFlag& SetApplyOn(FuncApplyOn* callback) { apply_on = callback; return *this; }
			EffectHandler_BoardFlag& SetRemoveFrom(FuncRemoveFrom* callback) { remove_from = callback; return *this; }

			bool IsCallbackSet_ApplyOn() const { return apply_on != nullptr; }
			bool IsCallbackSet_RemoveFrom() const { return remove_from != nullptr; }

			bool NoAppliedEnchantment() const { return !applied; }
			void AfterCopied() { applied = false; }

			void Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, bool aura_valid);

		private:
			FuncApplyOn * apply_on;
			FuncRemoveFrom * remove_from;

			bool applied;
		};
	}
}