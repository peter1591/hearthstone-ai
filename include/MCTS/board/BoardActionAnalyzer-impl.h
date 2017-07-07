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
			if (op_map_size_ == 0) {
				op_map_size_ = 0;

				auto const& hand = board.GetCurrentPlayer().hand_;
				if (!hand.Empty()) {
					op_map_[op_map_size_] = &BoardActionAnalyzer::PlayCard;
					++op_map_size_;
				}

				attackers_ = FlowControl::ValidActionGetter(board).GetAttackers();
				assert(attackers_.has_value());
				if (!attackers_->empty()) {
					op_map_[op_map_size_] = &BoardActionAnalyzer::Attack;
					++op_map_size_;
				}

				op_map_[op_map_size_] = &BoardActionAnalyzer::HeroPower;
				++op_map_size_;

				op_map_[op_map_size_] = &BoardActionAnalyzer::EndTurn;
				++op_map_size_;
			}

			return (int)op_map_size_;
		}

		inline Result BoardActionAnalyzer::ApplyAction(state::State & board, int action, RandomGenerator & random, ActionParameterGetter & action_parameters) {
			assert(op_map_size_ > 0);
			assert(action >= 0);
			assert(action < (int)op_map_size_);
			return (this->*op_map_[action])(board, random, action_parameters);
		}

		inline Result BoardActionAnalyzer::PlayCard(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters)
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
			assert(attackers_.has_value());
			assert([&]() {
				auto attackers = FlowControl::ValidActionGetter(board).GetAttackers();
				return attackers == *attackers_; // in fact, we don't care the ordering
													// but, the logic of 'GetAttackers()' returns
													// the same ordering for identical board views
			}());

			if (attackers_->empty()) return Result::kResultInvalid;

			FlowControl::FlowContext flow_context(random, action_parameters);
			assert(!attackers_->empty());
			int idx = action_parameters.GetNumber(ActionType::kChooseAttacker, (int)attackers_->size());

			int attacker_idx = (*attackers_)[idx];
			state::CardRef attacker;
			if (attacker_idx == 7) {
				attacker = board.GetCurrentPlayer().GetHeroRef();
			}
			else {
				assert(attacker_idx >= 0);
				assert(attacker_idx < 7);
				attacker = board.GetCurrentPlayer().minions_.Get((size_t)(attacker_idx));
			}

			return FlowControl::FlowController(board, flow_context).Attack(attacker);
		}

		inline Result BoardActionAnalyzer::HeroPower(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters)
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			return FlowControl::FlowController(board, flow_context).HeroPower();
		}

		inline Result BoardActionAnalyzer::EndTurn(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters)
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			return FlowControl::FlowController(board, flow_context).EndTurn();
		}
	}
}