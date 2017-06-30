#pragma once

#include <unordered_map>
#include <memory>

#include "board/Board.h"
#include "builder/TreeBuilder.h"
#include "MCTS/detail/BoardNodeMap.h"

namespace mcts
{
	// single observer MCTS
	// Need to support the action 'ApplyOtherTurnAction'
	// Implementation details:
	//    Use builder::TreeBuilder to construct the game tree in a Monte-Carlo fashion
	//    To support the action 'ApplyOtherTurnAction',
	//       we use the SOMCTS_TreeNodeAddon to create a hash table for some tree noes
	//       so we can find a node corresponds to the given board, and step on it
	class SOMCTS
	{		
	public:
		SOMCTS(state::PlayerSide side, Statistic<> & statistic) : side_(side), statistic_(statistic) {}

		void StartEpisode()
		{
			node_ = &root_;
			stage_ = kStageSelection;
			updater_.Clear();
		}

		// Include to perform the end-turn action at last to switch side
		Result PerformOwnTurnActions(board::Board & board)
		{
			assert(board.GetCurrentPlayer().GetSide() == side_);
			assert([](builder::TreeBuilder::TreeNode* node) {
				if (!node) return true;
				if (!node->GetActionType().IsValid()) return true;
				return node->GetActionType().GetType() == ActionType::kMainAction;
			}(node_));

			builder::TreeBuilder builder(statistic_);
			Result result = Result::kResultInvalid;

			builder.TurnStart(node_);

			while (board.GetCurrentPlayer().GetSide() == side_) {
				if (stage_ == kStageSimulation) {
					result = builder.PerformSimulate(board);
					assert(result != Result::kResultInvalid);
					if (result != Result::kResultNotDetermined) return result;
				}
				else {
					// Selection stage
					assert(stage_ == kStageSelection);
					assert([](builder::TreeBuilder::TreeNode* node) {
						if (!node) return false;
						if (!node->GetActionType().IsValid()) return true;
						return node->GetActionType().GetType() == ActionType::kMainAction;
					}(node_));
					auto perform_result = builder.PerformSelect(node_, board, &updater_);
					assert(perform_result.result != Result::kResultInvalid);
					assert([](builder::TreeBuilder::TreeNode* node) {
						if (!node->GetActionType().IsValid()) return true;
						return node->GetActionType().GetType() == ActionType::kMainAction;
					}(perform_result.node));

					result = perform_result.result;
					if (result != Result::kResultNotDetermined) return result;

					if (perform_result.new_node_created) {
						stage_ = kStageSimulation;
						node_ = nullptr;
					}
					else {
						node_ = perform_result.node;
					}
				}
			}
			assert(result == Result::kResultNotDetermined);
			return result;
		}

		// Another player finished his actions
		// Just jump to the node present the new state
		void ApplyOthersActions(board::Board const& board)
		{
			if (stage_ == kStageSimulation) return;

			assert(stage_ == kStageSelection);
			assert(node_);

			node_ = node_->GetAddon().board_node_map.GetOrCreateNode(board);
		}

		void EpisodeFinished(Result result)
		{
			// suppose AI is helping the first player
			bool credit = mcts::StaticConfigs::CreditPolicy::GetCredit(state::kPlayerFirst, result);
			updater_.Update(credit);
		}

	private:
		state::PlayerSide side_;
		builder::TreeBuilder::TreeNode root_;
		Statistic<> & statistic_;

	private: // traversal progress
		builder::TreeBuilder::TreeNode* node_;
		Stage stage_;
		builder::TreeUpdater updater_;
	};
}