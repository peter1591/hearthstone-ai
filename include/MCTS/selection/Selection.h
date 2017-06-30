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
			void StartNewAction(TreeNode * root) {
				path_.clear();
				StepNext(-1, nullptr, root);
				new_node_created_ = false;
				pending_randoms_ = false;
				saved_path_ = path_;
			}
			void RestartAction() { path_ = saved_path_; }

			// @return >= 0 for the chosen action; < 0 if no valid action
			int GetAction(
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

				TreeNode* current_node = GetCurrentNode();

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
				if (next_choice < 0) return -1; // all of the choices are invalid actions

				StepNext(next_choice, nullptr, nullptr); // pass nullptr to delay the node-creation as much as possible
												// since the node of the last action doesn't need to be created
				return next_choice;
			}

			void FinishMainAction(TreeNode* starting_node, board::Board const& board, bool * created_new_node) {
				assert([&]() {
					// the last node of a main action is not necessary to be created
					// since we're about to use the hash table to find the correct node
					return path_.back().node == nullptr;
				}());

				if (pending_randoms_) {
					// last actions are random nodes, so no tree node are created
					// use hash table to find which node should be our next node
					// this way, we can share tree node for identical states
					assert(starting_node->GetActionType().GetType() == ActionType::kMainAction);
					TreeNode* next_node = starting_node->GetAddon().board_node_map.GetOrCreateNode(board, &new_node_created_);
					StepNext(-1, // -1 indicates a random move
						nullptr, // random action has no edge
						next_node);
				}

				*created_new_node = new_node_created_;
			}

			void ReportInvalidAction() {
				auto it = path_.rbegin();
				
				assert(it != path_.rend());
				TreeNode* child = it->node;
				TreeNode* parent = nullptr;
				int edge = it->leading_choice;

				while (true) {
					++it;
					assert(it != path_.rend()); // we should be able to find a blame node along the path
					parent = it->node;

					// if a sub-action failed, it means the main action failed.
					//    More precisely, it means the calling to FlowController is failed
					//    not only the callback like 'GetTarget()' or 'ChooseOne()' is failed
					//    so we find the node to blame, and remove it from its parent
					if (parent->GetActionType().IsInvalidStateBlameNode()) break;

					// look up further to find the blame node
					child = it->node;
					edge = it->leading_choice;
				}

				parent->MarkChildInvalid(edge, child);
			}

			std::vector<TraversedNodeInfo> const& GetTraversedPath() const { return path_; }

			TreeNode* GetCurrentNode() {
				assert(!path_.empty());
				auto it = path_.rbegin();

				if (!it->node) {
					// delay node creation to here (as late as possible)
					auto it_parent = it;
					++it_parent;
					assert(it_parent != path_.rend());

					auto result = it_parent->node->GetOrCreateChild(it->leading_choice);
					new_node_created_ = result.second;
					TreeNode::ChildType & child = result.first->second;
					it->edge_addon = &child.first;
					it->node = child.second.get();
				}

				assert(it->node);
				return it->node;
			}

			void StepNext(int leading_choice, EdgeAddon* leading_edge_addon, TreeNode* next_node)
			{
				if (!path_.empty()) GetCurrentNode(); // ensure the delay-create node is actually created
				path_.push_back({ leading_choice, leading_edge_addon, next_node });
			}

		private:
			std::vector<TraversedNodeInfo> path_;

			std::vector<TraversedNodeInfo> saved_path_;

			bool new_node_created_;
			bool pending_randoms_;
		};
	}
}
