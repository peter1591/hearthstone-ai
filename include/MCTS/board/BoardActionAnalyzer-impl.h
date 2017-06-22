#pragma once

#include "MCTS/board/Board.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"
#include "FlowControl/ValidActionGetter.h"

namespace mcts
{
	namespace board
	{
		inline int BoardActionAnalyzer::GetActionsCount(Board const& board)
		{
			FlowControl::ValidActionGetter valid_action_getter(board);

			int idx = 0;

			playable_cards_ = valid_action_getter.GetPlayableCards();
			if (!playable_cards_.empty()) {
				actions_[idx++] = &BoardActionAnalyzer::PlayCard;
			}

			attackers_ = valid_action_getter.GetAttackers();
			if (!attackers_.empty()) {
				actions_[idx++] = &BoardActionAnalyzer::Attack;
			}

			if (valid_action_getter.HeroPowerUsable()) {
				actions_[idx++] = &BoardActionAnalyzer::HeroPower;
			}

			actions_[idx++] = &BoardActionAnalyzer::EndTurn;

			return idx;
		}

		inline Result BoardActionAnalyzer::ApplyAction(Board & board, int action, RandomGenerator & random, ActionParameterGetter & action_parameters) {
			return (this->*(actions_[action]))(board, random, action_parameters);
		}

		inline Result BoardActionAnalyzer::PlayCard(Board & board, RandomGenerator & random, ActionParameterGetter & action_parameters)
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			assert(!playable_cards_.empty());
			int idx = action_parameters.GetNumber(ActionType::kChooseHandCard, (int)playable_cards_.size());
			int hand_idx = playable_cards_[idx];
			return FlowControl::FlowController(board, flow_context).PlayCard(hand_idx);
		}

		inline Result BoardActionAnalyzer::Attack(Board & board, RandomGenerator & random, ActionParameterGetter & action_parameters)
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			assert(!attackers_.empty());
			int idx = action_parameters.GetNumber(ActionType::kChooseAttacker, (int)attackers_.size());
			state::CardRef attacker = attackers_[idx];

			return FlowControl::FlowController(board, flow_context).Attack(attacker);
		}

		inline Result BoardActionAnalyzer::HeroPower(Board & board, RandomGenerator & random, ActionParameterGetter & action_parameters)
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			return FlowControl::FlowController(board, flow_context).HeroPower();
		}

		inline Result BoardActionAnalyzer::EndTurn(Board & board, RandomGenerator & random, ActionParameterGetter & action_parameters)
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			return FlowControl::FlowController(board, flow_context).EndTurn();
		}
	}
}