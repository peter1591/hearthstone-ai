#pragma once

#include "engine/FlowControl/aura/EffectHandler_BoardFlag.h"

namespace engine {
	namespace FlowControl
	{
		namespace aura
		{
			inline void EffectHandler_OwnerPlayerFlag::Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, bool aura_valid)
			{
				assert(apply_on);
				assert(remove_from);

				if (!aura_valid) {
					if (!applied_player.IsValid()) return; // no change

					(*remove_from)({ Manipulate(state, flow_context), card_ref, applied_player });
				}
				else {
					state::PlayerIdentifier owner = state.GetCard(card_ref).GetPlayerIdentifier();
					if (applied_player == owner) return; // no change

					(*apply_on)({ Manipulate(state, flow_context), card_ref, owner });
				}
			}
		}
	}
}