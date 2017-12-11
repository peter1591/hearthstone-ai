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
				op_map_[op_map_size_] = &BoardActionAnalyzer::PlayCard;
				++op_map_size_;
			}

			attackers_.clear();
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

		inline Result BoardActionAnalyzer::ApplyAction(FlowControl::FlowContext & flow_context, FlowControl::CurrentPlayerStateView board, IRandomGenerator & random, FlowControl::utils::ActionApplier::IActionParameterGetter & action_parameters) {
			std::shared_lock<Utils::SharedSpinLock> lock(mutex_);

			auto main_op = action_parameters.ChooseMainOp();

			switch (main_op) {
			case FlowControl::utils::MainOpType::kMainOpPlayCard:
				return PlayCard(flow_context, board, random, action_parameters);
			case FlowControl::utils::MainOpType::kMainOpAttack:
				return Attack(flow_context, board, random, action_parameters);
			case FlowControl::utils::MainOpType::kMainOpHeroPower:
				return HeroPower(flow_context, board, random, action_parameters);
			case FlowControl::utils::MainOpType::kMainOpEndTurn:
				return EndTurn(flow_context, board, random, action_parameters);
			default:
				assert(false);
				return Result::kResultInvalid;
			}
		}

		inline Result BoardActionAnalyzer::PlayCard(FlowControl::FlowContext & flow_context, FlowControl::CurrentPlayerStateView & board, IRandomGenerator & random, FlowControl::utils::ActionApplier::IActionParameterGetter & action_parameters)
		{
			assert(!playable_cards_.empty());
			int idx = 0;
			if (playable_cards_.size() >= 2) {
				idx = action_parameters.ChooseHandCard(playable_cards_);
				if (idx < 0) return Result::kResultInvalid;
			}
			size_t hand_idx = playable_cards_[idx];

			flow_context.SetCallback(random, action_parameters);
			return ConvertResult(board.PlayCard(flow_context, (int)hand_idx));
		}

		inline Result BoardActionAnalyzer::Attack(FlowControl::FlowContext & flow_context, FlowControl::CurrentPlayerStateView & board, IRandomGenerator & random, FlowControl::utils::ActionApplier::IActionParameterGetter & action_parameters)
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
			int idx = 0;
			if (attackers_.size() >= 2) {
				idx = action_parameters.ChooseAttacker(attackers_);
			}
			if (idx < 0) return Result::kResultInvalid;

			flow_context.SetCallback(random, action_parameters);
			return ConvertResult(board.Attack(flow_context, attackers_[idx]));
		}

		inline Result BoardActionAnalyzer::HeroPower(FlowControl::FlowContext & flow_context, FlowControl::CurrentPlayerStateView & board, IRandomGenerator & random, FlowControl::utils::ActionApplier::IActionParameterGetter & action_parameters)
		{
			flow_context.SetCallback(random, action_parameters);
			return ConvertResult(board.HeroPower(flow_context));
		}

		inline Result BoardActionAnalyzer::EndTurn(FlowControl::FlowContext & flow_context, FlowControl::CurrentPlayerStateView & board, IRandomGenerator & random, FlowControl::utils::ActionApplier::IActionParameterGetter & action_parameters)
		{
			flow_context.SetCallback(random, action_parameters);
			return ConvertResult(board.EndTurn(flow_context));
		}
	}
}