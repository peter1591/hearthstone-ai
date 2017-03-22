#pragma once

#include "FlowControl/flag_aura/Handler.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace flag_aura
	{
		inline bool Handler::Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card)
		{
			assert(this == &card.GetRawData().flag_aura_handler);
			assert(is_valid != nullptr);
			assert(apply);
			assert(remove);

			bool aura_valid = IsValid(state, flow_context, card_ref, card);

			if (aura_valid) {
				(*apply)({ state, flow_context, card_ref, card, *this });
				return true;
			}
			else {
				(*remove)({ state, flow_context, card_ref, card, *this });
				return false;
			}
		}

		inline void Handler::AfterCopied()
		{
			applied_player.InValidate();
		}

		inline bool Handler::IsValid(
			state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card)
		{
			if (card.GetRawData().silenced) return false;
			return (*is_valid)({ state, flow_context, card_ref, card, *this});
		}
	}
}