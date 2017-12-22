#pragma once

#include "engine/FlowControl/aura/EffectHandler_BoardFlag.h"

namespace engine {
	namespace FlowControl
	{
		namespace aura
		{
			inline void EffectHandler_BoardFlag::Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, bool aura_valid)
			{
				assert(apply_on);
				assert(remove_from);

				if (aura_valid == applied) return; // no change

				if (aura_valid) {
					(*apply_on)({ Manipulate(state, flow_context), card_ref });
					applied = true;
				}
				else {
					(*remove_from)({ Manipulate(state, flow_context), card_ref });
					applied = false;
				}
			}
		}
	}
}