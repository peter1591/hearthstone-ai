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
		SOMCTS(state::PlayerSide side, builder::TreeBuilder::TreeNode & root, Statistic<> & statistic,
			std::mt19937 & selection_rand, std::mt19937 & simulation_rand)
			:
			side_(side), root_(root), statistic_(statistic),
			builder_(side, *this, statistic_, selection_rand, simulation_rand),
			node_(nullptr), stage_(Stage::kStageSelection), updater_()
		{}

		SOMCTS(SOMCTS const&) = delete;
		SOMCTS & operator=(SOMCTS const&) = delete;

		void StartEpisode()
		{
			node_ = &root_;
			stage_ = kStageSelection;
			updater_.Clear();
		}

		// Include to perform the end-turn action at last to switch side
		Result PerformOwnTurnActions(board::Board const& board)
		{
			assert(side_ == board.GetViewSide()); // prevent information leak
			assert(board.GetCurrentPlayer().GetSide() == side_);

			Result result = Result::kResultInvalid;

			detail::BoardNodeMap * turn_node_map = nullptr;
			if (stage_ == kStageSelection) {
				assert(node_);
				assert([](builder::TreeBuilder::TreeNode* node) {
					if (!node) return false;
					if (!node->GetActionType().IsValid()) return true;
					return node->GetActionType().GetType() == ActionType::kMainAction;
				}(node_));
				turn_node_map = &node_->GetAddon().board_node_map;
			}

			while (board.GetCurrentPlayer().GetSide() == side_) {
				if (stage_ == kStageSimulation) {
					result = builder_.PerformSimulate(board);
					assert(result.type_ != Result::kResultInvalid);
					if (result.type_ != Result::kResultNotDetermined) return result;
				}
				else {
					// Selection stage
					assert(stage_ == kStageSelection);
					assert([](builder::TreeBuilder::TreeNode* node) {
						if (!node) return false;
						if (!node->GetActionType().IsValid()) return true;
						return node->GetActionType().GetType() == ActionType::kMainAction;
					}(node_));
					auto perform_result = builder_.PerformSelect(node_, board, *turn_node_map, &updater_);
					assert(perform_result.result.type_ != Result::kResultInvalid);
					
					result = perform_result.result;
					if (result.type_ != Result::kResultNotDetermined) return result;

					assert([](builder::TreeBuilder::TreeNode* node) {
						if (!node) return false;
						if (!node->GetActionType().IsValid()) return true;
						return node->GetActionType().GetType() == ActionType::kMainAction;
					}(perform_result.node));

					if (perform_result.change_to_simulation) {
						stage_ = kStageSimulation;
						node_ = nullptr;
					}
					else {
						node_ = perform_result.node;
					}
				}
			}
			assert(result.type_ == Result::kResultNotDetermined);
			return result;
		}

		// Another player finished his actions
		// Just jump to the node present the new state
		void ApplyOthersActions(board::Board const& board)
		{
			assert(side_ == board.GetViewSide()); // prevent information leak

			if (stage_ == kStageSimulation) return;

			assert(stage_ == kStageSelection);
			assert(node_);

			node_ = node_->GetAddon().board_node_map.GetOrCreateNode(board);
		}

		void EpisodeFinished(state::State const& state, Result result)
		{
			double credit = mcts::StaticConfigs::CreditPolicy::GetCredit(state, result);
			assert(credit >= 0.0);
			assert(credit <= 1.0); // TODO: should take into account episilon precision
			if (side_ == state::kPlayerSecond) {
				credit = 1.0 - credit;
			}
			updater_.Update(credit);
		}
		
		int ChooseAction(ActionType action_type, board::ActionChoices const& choices) {
			if (stage_ == kStageSelection) {
				return builder_.ChooseSelectAction(action_type, choices);
			}
			else {
				assert(stage_ == kStageSimulation);
				return builder_.ChooseSimulateAction(action_type, choices);
			}
		}

		auto GetRootNode() const { return &root_; }

	private:
		const state::PlayerSide side_;
		builder::TreeBuilder::TreeNode & root_;
		Statistic<> & statistic_;

	private: // traversal progress
		builder::TreeBuilder builder_;
		builder::TreeBuilder::TreeNode* node_;
		Stage stage_;
		builder::TreeUpdater updater_;
	};
}