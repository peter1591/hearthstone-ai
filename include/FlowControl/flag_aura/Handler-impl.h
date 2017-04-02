#pragma once

#include "FlowControl/flag_aura/Handler.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace flag_aura
	{
		inline bool Handler::Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
		{
			assert(is_valid != nullptr);
			assert(apply);
			assert(remove);

			bool aura_valid = IsValid(state, flow_context, card_ref);

			if (aura_valid) {
				(*apply)({ Manipulate(state, flow_context), card_ref, *this });
				return true;
			}
			else {
				(*remove)({ Manipulate(state, flow_context), card_ref, *this });
				return false;
			}
		}

		inline void Handler::AfterCopied()
		{
			applied_player.InValidate();
			applied_ref.Invalidate();
		}

		inline bool Handler::IsValid(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
		{
			if (state.GetCard(card_ref).IsSilenced()) return false;
			return (*is_valid)({ Manipulate(state, flow_context), card_ref, *this});
		}
	}
}