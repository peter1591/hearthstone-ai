#pragma once

#include <unordered_map>
#include <memory>

#include "board/Board.h"
#include "builder/TreeBuilder.h"
#include "MCTS/detail/SOMCTS_TreeNodeAddon.h"

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

			builder::TreeBuilder builder(statistic_);
			builder::TreeBuilder::PerformResult result;

			while (board.GetCurrentPlayer().GetSide() == side_) {
				if (stage_ == kStageSimulation) {
					result = builder.PerformSimulate(board);
					assert(result.result != Result::kResultInvalid);
					if (result.result != Result::kResultNotDetermined) return result.result;
				}
				else {
					// Selection stage
					assert(stage_ == kStageSelection);
					result = builder.PerformSelect(node_, board, &updater_);
					assert(result.result != Result::kResultInvalid);
					if (result.result != Result::kResultNotDetermined) return result.result;

					node_ = result.node;

					if (result.new_node_created) {
						stage_ = kStageSimulation;
						node_ = nullptr;
					}
					else {
						node_ = result.node;
					}
				}
			}
			assert(result.result == Result::kResultNotDetermined);
			return result.result;
		}

		// Another player finished his actions
		// Just jump to the node present the new state
		void ApplyOthersActions(board::Board const& board)
		{
			if (stage_ == kStageSimulation) return;

			assert(stage_ == kStageSelection);
			assert(node_);

			node_ = node_->GetAddon().somcts.GetOrCreateNode(board);
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