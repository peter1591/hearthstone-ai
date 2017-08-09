#pragma once

#include <set>

#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"
#include "FlowControl/FlowController.h"
#include "FlowControl/ValidActionGetter.h"

namespace mcts
{
	namespace board
	{
		inline int BoardActionAnalyzer::GetActionsCount(FlowControl::CurrentPlayerStateView const& board)
		{
			{
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				if (op_map_size_ > 0) return (int)op_map_size_;
			}

			std::lock_guard<Utils::SharedSpinLock> write_lock(mutex_);
			if (op_map_size_ > 0) return (int)op_map_size_;

			assert(playable_cards_.empty());
			board.ForEachPlayableCard([&](size_t idx) {
				playable_cards_.push_back(idx);
				return true;
			});
			if (!playable_cards_.empty()) {
				op_map_[op_map_size_] = &BoardActionAnalyzer::PlayCard;
				++op_map_size_;
			}

			assert(attackers_.empty());
			board.ForEachAttacker([&](int idx) { 
				attackers_.push_back(idx);
				return true;
			});
			if (!attackers_.empty()) {
				op_map_[op_map_size_] = &BoardActionAnalyzer::Attack;
				++op_map_size_;
			}

			if (board.CanUseHeroPower()) {
				op_map_[op_map_size_] = &BoardActionAnalyzer::HeroPower;
				++op_map_size_;
			}

			op_map_[op_map_size_] = &BoardActionAnalyzer::EndTurn;
			++op_map_size_;
			return (int)op_map_size_;
		}

		inline Result BoardActionAnalyzer::ApplyAction(FlowControl::CurrentPlayerStateView board, int action, IRandomGenerator & random, IActionParameterGetter & action_parameters) {
			std::shared_lock<Utils::SharedSpinLock> lock(mutex_);

			assert(op_map_size_ > 0);
			assert(action >= 0);
			assert(action < (int)op_map_size_);
			return (this->*op_map_[action])(board, random, action_parameters);
		}

		inline Result BoardActionAnalyzer::PlayCard(FlowControl::CurrentPlayerStateView & board, IRandomGenerator & random, IActionParameterGetter & action_parameters)
		{
			assert(!playable_cards_.empty());
			int idx = 0;
			if (playable_cards_.size() >= 2) {
				idx = action_parameters.GetNumber(ActionType::kChooseHandCard, ActionChoices((int)playable_cards_.size()));
				if (idx < 0) return Result::kResultInvalid;
			}
			size_t hand_idx = playable_cards_[idx];

			FlowControl::FlowContext flow_context(random, action_parameters);
			return board.PlayCard(flow_context, (int)hand_idx);
		}

		inline Result BoardActionAnalyzer::Attack(FlowControl::CurrentPlayerStateView & board, IRandomGenerator & random, IActionParameterGetter & action_parameters)
		{
			assert([&]() {
				std::set<int> current;
				for (int idx : attackers_) current.insert(idx);
				std::set<int> checking;
				board.ForEachAttacker([&](int idx) {
					checking.insert(idx);
					return true;
				});
				return current == checking;
			}());

			if (attackers_.empty()) return Result::kResultInvalid;

			assert(!attackers_.empty());
			int idx = action_parameters.GetNumber(ActionType::kChooseAttacker, (int)attackers_.size());
			if (idx < 0) return Result::kResultInvalid;

			FlowControl::FlowContext flow_context(random, action_parameters);
			return board.Attack(flow_context, attackers_[idx]);
		}

		inline Result BoardActionAnalyzer::HeroPower(FlowControl::CurrentPlayerStateView & board, IRandomGenerator & random, IActionParameterGetter & action_parameters)
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			return board.HeroPower(flow_context);
		}

		inline Result BoardActionAnalyzer::EndTurn(FlowControl::CurrentPlayerStateView & board, IRandomGenerator & random, IActionParameterGetter & action_parameters)
		{
			FlowControl::FlowContext flow_context(random, action_parameters);
			return board.EndTurn(flow_context);
		}
	}
}