#pragma once

#include "FlowControl/battlecry/Handler.h"

#include "FlowControl/battlecry/Contexts-impl.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace battlecry
	{
		inline bool Handler::PrepareBattlecryTarget(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const
		{
			if (!battlecry_target_getter) return true;

			state::targetor::TargetsGenerator targets_generator;
			if (!battlecry_target_getter({ state, flow_context, card_ref, card, targets_generator })) {
				return false;
			}
			flow_context.battlecry_target_ = flow_context.action_parameters_.GetBattlecryTarget(
				state, card_ref, card, targets_generator.GetInfo());
			return true;
		}

		inline void Handler::DoBattlecry(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const
		{
			if (!battlecry) return;
			(*battlecry)({ state, flow_context, card_ref, card });
		}
	}
}