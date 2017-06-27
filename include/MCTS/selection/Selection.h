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
				StepNext(-1, root);
				new_node_created_ = false;
			}

			// @return >= 0 for the chosen action; < 0 if no valid action
			int GetAction(
				board::Board const& board,
				ActionType action_type,
				board::ActionChoices const& choices)
			{
				std::pair<int, TreeNode*> next_info;
				if (action_type.IsChosenRandomly()) {
					assert(choices.GetType() == board::ActionChoices::kChooseFromZeroToExclusiveMax);
					int rnd_choice = StaticConfigs::SelectionPhaseRandomActionPolicy::GetRandom(choices.Size());
					if (GetCurrentNode() != nullptr) {
						// postpone the find of TreeNode on next non-random (sub)-action
						StepNext(-1, nullptr);
					}
					return rnd_choice;
				}

				assert(action_type.IsChosenManually());
				if (GetCurrentNode() == nullptr) {
					if (action_type.GetType() == ActionType::kMainAction) {
						assert(false); // main action should with a valid node
						return -1;
					}
					else {
						// TODO: we can swap the random actions to be done first
						// TODO: only choose-from-card can be after random?
						assert(false);
					}
				}
				next_info = SelectActionByChoice(action_type, board, choices);
				
				int next_choice = next_info.first;
				TreeNode* next_node = next_info.second;

				if (!next_node) {
					return -1; // all of the choices are invalid actions
				}

				StepNext(next_choice, next_node);
				return next_choice;
			}

			void FinishMainAction(TreeNode* starting_node, board::Board const& board, bool * created_new_node) {
				if (GetCurrentNode() == nullptr) {
					// last actions are random nodes, so no tree node are created
					// use hash table to find which node should be our next node
					// this way, we can share tree node for identical states
					TreeNode* next_node = starting_node->GetAddon().board_node_map.GetOrCreateNode(board, &new_node_created_);
					SetCurrentNode(next_node);
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

			TreeNode* GetCurrentNode() const { return path_.back().node; }
			void SetCurrentNode(TreeNode* node) {
				assert(!path_.back().node);
				path_.back().node = node;
			}

			void StepNext(int leading_choice, TreeNode* next_node)
			{
				path_.push_back({ leading_choice, next_node });
			}

		private:
			std::pair<int, TreeNode*> SelectActionByChoice(
				ActionType action_type, board::Board const& board, board::ActionChoices const& choices)
			{
				using PolicyHelper = StaticConfigs::SelectionPhaseSelectActionPolicy;
				return GetCurrentNode()->Select(action_type, choices, PolicyHelper(), &new_node_created_);
			}

		private:
			std::vector<TraversedNodeInfo> path_;

			std::vector<TraversedNodeInfo> saved_path_;

			bool new_node_created_;
		};
	}
}
