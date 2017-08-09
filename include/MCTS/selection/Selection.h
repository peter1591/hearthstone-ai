#pragma once

#include <assert.h>
#include "MCTS/Config.h"
#include "MCTS/selection/TreeNode.h"
#include "MCTS/selection/TraversedNodeInfo.h"

#include "MCTS/selection/ChildNodeMap-impl.h"

namespace mcts
{
	namespace selection
	{
		class Selection
		{
		public:
			Selection(std::mt19937 & rand) : path_(), random_(rand), new_node_created_(false), pending_randoms_(false) {}

			Selection(Selection const&) = delete;
			Selection & operator=(Selection const&) = delete;

			void StartNewMainAction(TreeNode * root) {
				path_.clear();
				path_.emplace_back(root);
				new_node_created_ = false;
				pending_randoms_ = false;
			}

			// @return >= 0 for the chosen action; < 0 if no valid action
			int ChooseAction(
				board::Board const& board,
				ActionType action_type,
				board::ActionChoices const& choices)
			{
				if (choices.Empty()) return -1;

				if (action_type.IsChosenRandomly()) {
					assert(choices.GetType() == board::ActionChoices::kChooseFromZeroToExclusiveMax);
					pending_randoms_ = true;
					return random_.GetRandom(choices.Size());
				}

				assert(action_type.IsChosenManually());
				assert(!path_.empty());
				if (path_.back().HasMadeChoice()) {
					// Note: Under a multi-threaded environment, a choice might be chosen to expand
					// from several threads at the same time. One of the threads might be faster, and
					// mark the choice as invalid/redirect first.
					// From another thread's view, the ConstructNextNode() will return a node with nullptr
					TreeNode* new_node = path_.back().ConstructNextNode(&new_node_created_);
					if (!new_node) {
						// TODO: remove this if no invalid node is possible
						return -1; // marked as invalid from another thread
					}
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

			void ReportInvalidAction() {
				auto it = path_.rbegin();

				// skip the last choice which yields an invalid state due to no valid child
				for (; it != path_.rend(); ++it) {
					if (it->GetChoice() >= 0) break;
				}

				for (; it != path_.rend(); ++it) {
					assert(it->GetChoice() >= 0);
					if (it->GetNode()->GetActionType().IsInvalidStateBlameNode()) {
						it->GetNode()->MarkChoiceInvalid(it->GetChoice());
						break;
					}
				}
			}

			std::vector<TraversedNodeInfo> const& GetTraversedPath() const { return path_; }
			std::vector<TraversedNodeInfo> & GetMutableTraversedPath() { return path_; }

			bool HasNewNodeCreated() const { return new_node_created_; }

		private:
			std::vector<TraversedNodeInfo> path_;
			StaticConfigs::SelectionPhaseRandomActionPolicy random_;
			bool new_node_created_;
			bool pending_randoms_;
		};
	}
}
