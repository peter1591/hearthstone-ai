#pragma once

#include <assert.h>
#include "MCTS/Config.h"
#include "MCTS/selection/TreeNode.h"
#include "MCTS/selection/TraversedNodeInfo.h"

namespace mcts
{
	namespace selection
	{
		class Selection
		{
		public:
			Selection() : root_(nullptr) {}

			void StartNewMainAction(TreeNode * root) {
				root_ = root;

				path_.clear();
				path_.emplace_back(root_);
				new_node_created_ = false;
				pending_randoms_ = false;
			}
			void RestartAction() {
				StartNewMainAction(root_);
			}

			// @return >= 0 for the chosen action; < 0 if no valid action
			int ChooseAction(
				board::Board const& board,
				ActionType action_type,
				board::ActionChoices const& choices)
			{
				if (action_type.IsChosenRandomly()) {
					assert(choices.GetType() == board::ActionChoices::kChooseFromZeroToExclusiveMax);
					pending_randoms_ = true;
					return StaticConfigs::SelectionPhaseRandomActionPolicy::GetRandom(choices.Size());
				}

				assert(action_type.IsChosenManually());
				assert(!path_.empty());
				if (path_.back().HasMadeChoice()) {
					TreeNode* new_node = path_.back().ConstructNextNode(&new_node_created_);
					path_.emplace_back(new_node);
				}

				assert(!path_.back().HasMadeChoice());
				TreeNode* current_node = path_.back().GetNode();

				if (pending_randoms_) {
					switch (action_type.GetType()) {
					case ActionType::kChooseOne:
						// choose-one card might be triggered after random
						break;

					default:
						// Not allow random actions to be conducted first before any sub-action
						// That is, the player
						//    1. Choose the hand card
						//    2. Choose minion put location
						// Then, random gets started
						// Similar to other sub-actions
						assert(false);
						return -1;
					}

					// Check if the board-view and sub-action-type are consistency
					// Currently, we have an assumption that
					//    * A random action can be swapped to the end of a sub-action sequence
					//      without any distinguishable difference
					// But, it may not be this case if...
					//    1. You have a 50% chance to choose a card
					//    2. Summon a random minion, then choose a card
					// In both cases, a random node should be added before the sub-action 'choose-a-card'
					// From a game-play view, that is to say
					//    The random outcome *influences* the player's decision
					// So, if this assertion failed
					//    It means a random node is necessary before this node
					assert(current_node->GetAddon().consistency_checker.Check(board, action_type, choices));
				}
				else {
					assert(current_node->GetAddon().consistency_checker.Check(board, action_type, choices));
				}

				int next_choice = current_node->Select(action_type, choices,
					StaticConfigs::SelectionPhaseSelectActionPolicy());
				if (next_choice < 0) {
					return -1; // all of the choices are invalid actions
				}

				path_.back().MakeChoice(next_choice);

				return next_choice;
			}

			TreeNode* FinishMainAction(
				detail::BoardNodeMap & last_node_lookup,
				board::Board const& board,
				bool * created_new_node)
			{
				TreeNode* final_node = last_node_lookup.GetOrCreateNode(board, created_new_node);
				if (new_node_created_) *created_new_node = true;

				path_.back().ConstructRedirectNode();

				return final_node;
			}

			void ReportInvalidAction() {
				for (auto it = path_.rbegin(); it != path_.rend(); ++it) {
					if (it->GetNode()->GetActionType().IsInvalidStateBlameNode()) {
						if (it->GetChoice() < 0) continue;
						it->GetNode()->MarkChoiceInvalid(it->GetChoice());
						return;
					}
				}
				assert(false);
			}

			std::vector<TraversedNodeInfo> const& GetTraversedPath() const { return path_; }

		private:
			TreeNode* root_;
			std::vector<TraversedNodeInfo> path_;

			bool new_node_created_;
			bool pending_randoms_;
		};
	}
}
