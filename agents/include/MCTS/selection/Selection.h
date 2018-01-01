#pragma once

#include <assert.h>
#include "MCTS/Config.h"
#include "MCTS/selection/TreeNode.h"
#include "MCTS/selection/TraversedNodeInfo.h"
#include "MCTS/selection/TreeUpdater.h"

#include "MCTS/selection/ChildNodeMap-impl.h"

namespace mcts
{
	namespace selection
	{
		class Selection
		{
		public:
			Selection(TreeNode & tree, std::mt19937 & rand) :
				root_(tree), turn_node_map_(nullptr), path_(), random_(rand), policy_(), updater_()
			{}

			Selection(Selection const&) = delete;
			Selection & operator=(Selection const&) = delete;

			auto GetRootNode() const { return &root_; }

			void StartIteration() {
				path_.Restart(&root_);
			}

			void StartActions() {
				auto current_node = path_.GetCurrentNode();
				assert(current_node);
				assert([&]() {
					if (!current_node) return false;
					if (!current_node->GetActionType().IsValid()) return true;
					return current_node->GetActionType().GetType() == engine::ActionType::kMainAction;
				}());
				turn_node_map_ = &current_node->GetAddon().board_node_map;
			}

			void StartAction(engine::view::Board const& board) {
				auto current_node = path_.GetCurrentNode();
				assert(current_node);
				assert([&]() {
					if (!current_node->GetActionType().IsValid()) return true;
					return current_node->GetActionType().GetType() == engine::ActionType::kMainAction;
				}());
				(void)board;
				assert(current_node->GetAddon().consistency_checker.CheckBoard(board.CreateView()));
				
				assert(turn_node_map_);
			}

			// @return >= 0 for the chosen action
			int ChooseAction(engine::ActionType action_type, engine::ActionChoices const& choices)
			{
				assert(!choices.Empty());

				if (action_type.IsChosenRandomly()) {
					assert(choices.CheckType<engine::ActionChoices::ChooseFromZeroToExclusiveMax>());
					return random_.GetRandom(choices.Size());
				}

				assert(action_type.IsChosenManually());
				if (path_.HasCurrentNodeMadeChoice()) {
					path_.ConstructNode();
					assert(!path_.HasCurrentNodeMadeChoice());
				}

				TreeNode* current_node = path_.GetCurrentNode();
				assert(current_node->GetAddon().consistency_checker.SetAndCheck(action_type, choices));

				int next_choice = current_node->Select(action_type, choices, policy_);
				assert(next_choice >= 0); // should report a valid action
				path_.MakeChoiceForCurrentNode(next_choice);

				return next_choice;
			}

			// @return  If we should switch to simulation stage
			bool FinishAction(engine::view::Board const& board, engine::Result result) {
				assert(path_.HasCurrentNodeMadeChoice()); // at least a ChooseAction() is called
				// mark the last action as a redirect node
				path_.ConstructRedirectNode(turn_node_map_, board, result);

				bool switch_to_simulation = false;
				if (result == engine::kResultNotDetermined) {
					switch_to_simulation = StaticConfigs::StageController::SwitchToSimulation(
						path_.HasNewNodeCreated(),
						path_.GetPath().back().edge_addon_->GetChosenTimes());
				}

				return switch_to_simulation;
			}

			void JumpToNodeWithBoard(engine::view::Board const& board) {
				path_.JumpToNode(board);
			}

			void FinishIteration(engine::view::Board const& board, engine::Result result)
			{
				double credit = StaticConfigs::CreditPolicy::GetCredit(board, result);
				assert(credit >= 0.0);
				assert(credit <= 1.0); // TODO: should take into account episilon precision
				updater_.Update(path_.GetPath(), path_.GetCurrentNode(), credit);
			}

		private:
			TreeNode & root_;
			detail::BoardNodeMap * turn_node_map_;
			TraversedNodesInfo path_;
			StaticConfigs::SelectionPhaseRandomActionPolicy random_;
			StaticConfigs::SelectionPhaseSelectActionPolicy policy_;
			TreeUpdater updater_;
		};
	}
}
