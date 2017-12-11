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

			playable_cards_.clear();
			board.ForEachPlayableCard([&](size_t idx) {
				playable_cards_.push_back(idx);
				return true;
			});
			if (!playable_cards_.empty()) {
				op_map_[op_map_size_] = FlowControl::utils::MainOpType::kMainOpPlayCard;
				++op_map_size_;
			}

			attackers_.clear();
			board.ForEachAttacker([&](int idx) { 
				attackers_.push_back(idx);
				return true;
			});
			if (!attackers_.empty()) {
				op_map_[op_map_size_] = FlowControl::utils::MainOpType::kMainOpAttack;
				++op_map_size_;
			}

			if (board.CanUseHeroPower()) {
				op_map_[op_map_size_] = FlowControl::utils::MainOpType::kMainOpHeroPower;
				++op_map_size_;
			}

			op_map_[op_map_size_] = FlowControl::utils::MainOpType::kMainOpEndTurn;
			++op_map_size_;
			return (int)op_map_size_;
		}
	}
}