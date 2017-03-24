#pragma once

#include "FlowControl/deathrattle/Handler.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace deathrattle
	{
		inline void Handler::TriggerAll(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
			for (auto deathrattle : deathrattles) {
				deathrattle(context::Deathrattle{ Manipulate(state, flow_context), card_ref, card });
			}
		}
	}
}