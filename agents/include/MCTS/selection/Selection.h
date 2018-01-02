#pragma once

#include <assert.h>
#include "MCTS/Config.h"
#include "MCTS/selection/TreeNode.h"
#include "MCTS/selection/TraversedNodesInfo.h"

namespace mcts
{
	namespace selection
	{
		class Selection
		{
		public:
			Selection(TreeNode & tree, std::mt19937 & rand) :
				root_(tree), board_changed_(false), redirect_node_map_(nullptr),
				path_(), random_(rand), policy_()
			{}

			Selection(Selection const&) = delete;
			Selection & operator=(Selection const&) = delete;

			auto GetRootNode() const { return &root_; }

			void StartIteration() {
				path_.Restart(&root_);
				board_changed_ = false;
				redirect_node_map_ = nullptr;
			}

			void StartAction(engine::view::Board const& board) {
				if (board_changed_) {
					path_.JumpToNode(board);
					board_changed_ = false;
				}

				auto current_node = path_.GetCurrentNode();
				assert(current_node);
				assert(current_node->GetAddon().consistency_checker.CheckActionType(engine::ActionType::kMainAction));
				(void)board;
				assert(current_node->GetAddon().consistency_checker.CheckBoard(board.CreateView()));

				if (redirect_node_map_ == nullptr) {
					redirect_node_map_ = &current_node->GetAddon().board_node_map;
					assert(redirect_node_map_);
				}
			}

			// @return >= 0 for the chosen action
			int ChooseAction(engine::ActionType action_type, engine::ActionChoices & choices)
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

				int next_choice = current_node->GetChildren([&](ChildNodeMap const& children) {
					return policy_.SelectChoice(
						mcts::policy::selection::ChoiceIterator(choices, children));
				});

				assert(next_choice >= 0); // should report a valid action
				path_.MakeChoiceForCurrentNode(next_choice);

				return next_choice;
			}

			// @return  If we should switch to simulation stage
			bool FinishAction(engine::view::Board const& board, engine::Result result) {
				assert(path_.HasCurrentNodeMadeChoice()); // at least a ChooseAction() should be called

				// We tackle the randomness by using a board-node-map.
				// This *flatten* the tree structure, and effectively *forgot* the history
				// (Note that history here referring to the parent nodes of this node)
				assert(redirect_node_map_);
				path_.ConstructRedirectNode(redirect_node_map_, board, result);

				bool switch_to_simulation = false;
				if (result == engine::kResultNotDetermined) {
					switch_to_simulation = StaticConfigs::StageController::SwitchToSimulation(
						path_.HasNewNodeCreated(),
						path_.GetPath().back().edge_addon_->GetChosenTimes());
				}

				return switch_to_simulation;
			}

			void ApplyOthersActions(engine::view::Board const& board) {
				board_changed_ = true;

				// Preserve the history when any other player performed their actions
				// So we need another redirect node
				redirect_node_map_ = nullptr;
			}

			void FinishIteration(engine::view::Board const& board, engine::Result result)
			{
				double credit = StaticConfigs::CreditPolicy::GetCredit(board, result);
				assert(credit >= 0.0);
				assert(credit <= 1.0); // TODO: should take into account episilon precision
				path_.Update(credit);
			}

		private:
			TreeNode & root_;
			bool board_changed_;
			detail::BoardNodeMap * redirect_node_map_; // To tackle randomness, the next node is found from this map after each action
			TraversedNodesInfo path_;
			StaticConfigs::SelectionPhaseRandomActionPolicy random_;
			StaticConfigs::SelectionPhaseSelectActionPolicy policy_;
		};
	}
}
