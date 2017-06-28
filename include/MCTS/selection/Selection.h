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
			void StartNewAction() { saved_path_ = path_; }
			void RestartAction() { path_ = saved_path_; }

			void StartMainAction(TreeNode * root)
			{
				path_.clear();
				StepNext(-1, nullptr, root);
				new_node_created_ = false;
				pending_randoms_ = false;
			}

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
				if (pending_randoms_) {
					if (action_type.GetType() == ActionType::kMainAction) {
						assert(false); // main action should with a valid node
						return -1;
					}
					else if (action_type.GetType() == ActionType::kChooseOne) {
						// TODO: allow choose-from-card after random
						assert(false);
						return -1;
					}
					else {
						// Not allow random actions to be conducted first before any sub-action
						assert(false);
						return -1;
					}
				}

				int next_choice = GetCurrentNode()->Select(action_type, choices,
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
