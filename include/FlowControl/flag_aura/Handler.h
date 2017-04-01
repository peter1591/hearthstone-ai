#pragma once

#include "FlowControl/flag_aura/Contexts.h"

namespace FlowControl
{
	namespace flag_aura
	{
		class Handler
		{
		public:
			typedef bool FuncIsValid(contexts::AuraIsValid context);
			typedef void FuncApply(contexts::AuraApply context);
			typedef void FuncRemove(contexts::AuraRemove context);

			Handler() : is_valid(nullptr), apply(nullptr), remove(nullptr) {}

		public:
			void SetCallback_IsValid(FuncIsValid* callback) { is_valid = callback; }
			void SetCallback_Apply(FuncApply* callback) { apply = callback; }
			void SetCallback_Remove(FuncRemove* callback) { remove = callback; }

			bool IsCallbackSet_IsValid() const { return is_valid != nullptr; }
			bool IsCallbackSet_Apply() const { return apply != nullptr; }
			bool IsCallbackSet_Remove() const { return remove != nullptr; }

		public:
			bool NoAppliedEffect() const { return !applied_player.IsValid(); }
			bool Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref);
			void AfterCopied();

		private:
			bool IsValid(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref);

		public:
			state::PlayerIdentifier applied_player;

		private:
			FuncIsValid * is_valid;
			FuncApply * apply;
			FuncRemove * remove;
		};
	}
}