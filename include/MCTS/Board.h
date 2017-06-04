#pragma once

#include "state/State.h"
#include "FlowControl/FlowController.h"

namespace mcts
{
	using Result = FlowControl::Result;

	class Board
	{
	public:
		Board(state::State const& state, state::IRandomGenerator & random, FlowControl::IActionParameterGetter & action_parameters)
			: state_(state), flow_context_(random, action_parameters)
		{
		}

		Result PlayCard(int hand_idx) {
			return FlowControl::FlowController(state_, flow_context_).PlayCard(hand_idx);
		}
		Result EndTurn() {
			return FlowControl::FlowController(state_, flow_context_).EndTurn();
		}
		Result Attack(state::CardRef attacker, state::CardRef defender) {
			return FlowControl::FlowController(state_, flow_context_).Attack(attacker, defender);
		}
		Result HeroPower() {
			return FlowControl::FlowController(state_, flow_context_).HeroPower();
		}

		state::State const& GetState() const { return state_; }

	private:
		state::State state_;
		FlowControl::FlowContext flow_context_;
	};
}