#pragma once

namespace FlowControl {
	class FlowContext;

	namespace aura {
		namespace contexts {
			struct AuraApplyFlagOnOwnerPlayer;
			struct AuraRemoveFlagFromOwnerPlayer;
		}

		class EffectHandler_OwnerPlayerFlag {
		public:
			typedef void FuncApplyOn(contexts::AuraApplyFlagOnOwnerPlayer const& context);
			typedef void FuncRemoveFrom(contexts::AuraRemoveFlagFromOwnerPlayer const& context);

			EffectHandler_OwnerPlayerFlag() : apply_on(nullptr), remove_from(nullptr), applied_player() {}

			EffectHandler_OwnerPlayerFlag& SetApplyOn(FuncApplyOn* callback) { apply_on = callback; return *this; }
			EffectHandler_OwnerPlayerFlag& SetRemoveFrom(FuncRemoveFrom* callback) { remove_from = callback; return *this; }

			bool IsCallbackSet_ApplyOn() const { return apply_on != nullptr; }
			bool IsCallbackSet_RemoveFrom() const { return remove_from != nullptr; }

			bool NoAppliedEnchantment() const { return !applied_player.IsValid(); }
			void AfterCopied() { applied_player.InValidate(); }

			void Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, bool aura_valid);

		private:
			FuncApplyOn * apply_on;
			FuncRemoveFrom * remove_from;

			state::PlayerIdentifier applied_player;
		};
	}
}