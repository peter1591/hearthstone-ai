#pragma once
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"
#include "FlowControl/ValidActionGetter.h"

namespace mcts
{
	namespace board
	{
		inline int BoardActionAnalyzer::GetActionsCount(state::State const& board)
		{
			// TODO: remove play-card if hand's empty
			return 4;
		}

		inline Result BoardActionAnalyzer::ApplyAction(state::State & board, int action, RandomGenerator & random, ActionParameterGetter & action_parameters) {
			switch (action) {
			case 0:
				return PlayCard(board, random, action_parameters);
			case 1:
				return Attack(board, random, action_parameters);
			case 2:
				return HeroPower(board, random, action_parameters);
			case 3:
				return EndTurn(board, random, action_parameters);
			}
			assert(false);
			return Result::kResultInvalid;
		}

		inline Result BoardActionAnalyzer::PlayCard(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters) const
		{
			int idx = action_parameters.GetNumber(ActionType::kChooseHandCard, [&board]() {
				auto const& hand = board.GetCurrentPlayer().hand_;
				return ActionChoices((int)hand.Size());
			});
			if (idx < 0) return Result::kResultInvalid; // all cards are not playable
			FlowControl::FlowContext flow_context(random, action_parameters);
			return FlowControl::FlowController(board, flow_context).PlayCard(idx);
		}

		inline Result BoardActionAnalyzer::Attack(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters)
		{
			if (!attackers_.has_value()) {
				// TODO: to record attacker, we should record its index
				//    0: the hero
				//    1~7: the minion index (started from 1)
				// Rather than recording the state::CardRef
				attackers_ = FlowControl::ValidActionGetter(board).GetAttackers();
			}
			else {
				assert([&]() {
					auto attackers = FlowControl::ValidActionGetter(board).GetAttackers();
					return attackers == *attackers_;
				}());
			}

			assert(attackers_.has_value());
			if (attackers_->empty()) return Result::kResultInvalid;

			FlowControl::FlowContext flow_context(random, action_parameters);
			assert(!attackers_->empty());
			int idx = action_parameters.GetNumber(ActionType::kChooseAttacker, (int)attackers_->size());
			state::CardRef attacker = (*attackers_)[idx];

			return FlowControl::FlowController(board, flow_context).Attack(attacker);
		}

		inline Result BoardActionAnalyzer::HeroPower(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters) const
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			return FlowControl::FlowController(board, flow_context).HeroPower();
		}

		inline Result BoardActionAnalyzer::EndTurn(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters) const
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			return FlowControl::FlowController(board, flow_context).EndTurn();
		}
	}
}