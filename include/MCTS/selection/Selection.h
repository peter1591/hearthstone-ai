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

			void StartEpisode(TreeNode * root)
			{
				path_.clear();
				StepNext(-1, root);
			}

			// @return >= 0 for the chosen action; < 0 if no valid action
			int GetAction(board::Board const& board, ActionType action_type, int choices, bool * created_new_node)
			{
				ReportActionInfo(action_type, choices);
				
				auto next_info = SelectAction(board, action_type, choices, created_new_node);
				
				int next_choice = next_info.first;
				TreeNode* next_node = next_info.second;

				assert(next_choice >= 0);
				assert(next_node);

				StepNext(next_choice, next_node);

				return next_choice;
			}

			void ReportInvalidAction() {
				assert(!path_.empty());
				assert(path_.back().node == GetCurrentNode());
				
				bool node_removed = false;
				auto op = [node_removed](TreeNode* parent, int edge, TreeNode* child) mutable {
					assert(parent->GetAction(edge) == child);

					bool do_remove = false;
					if (!node_removed) do_remove = true;
					else {
						if (!child->ExpandDone()) {}
						else if (child->HasAnyValidAction()) {}
						else 
							do_remove = true;
					}

					if (!do_remove) return; // TODO: should early stop? no need to do traverse anymore
					
					parent->MarkInvalidAction(edge);
					if (!node_removed) node_removed = true;
				};

				auto it_prev = path_.rbegin();
				auto it = it_prev;
				++it;
				while (it != path_.rend()) {
					op(it->node, it_prev->leading_choice, it_prev->node);
					++it;
					++it_prev;
				}
			}

			std::vector<TraversedNodeInfo> const& GetTraversedPath() const { return path_; }

			TreeNode* GetCurrentNode() const { return path_.back().node; }

		private:
			void ReportActionInfo(ActionType action_type, int choices) {
				GetCurrentNode()->FillActions(action_type, choices);
			}

			std::pair<int, TreeNode*> SelectAction(board::Board const& board, ActionType action_type, int choices, bool * new_node) {
				// Check if current tree node has un-expanded action
				//   If yes, choose that action
				int next_action = GetCurrentNode()->GetNextActionToExpand();
				if (next_action >= 0) {
					*new_node = true;
					return { next_action, GetCurrentNode()->CreateAction(next_action) };
				}
				*new_node = false;

				if (!GetCurrentNode()->HasAnyValidAction()) {
					// no valid action from this node (all actions are removed 
					//    since they yields an invalid state)
					// However, if a (parent) node is with no valid child node
					// we should have deleted the (parent) node when the child node
					// got removed.
					assert(false);
					return { -1, nullptr };
				}

				if (action_type.IsChosenRandomly()) return SelectActionByRandom();
				else return SelectActionByChoice(board);
			}

			void StepNext(int leading_choice, TreeNode* next_node)
			{
				path_.push_back({ leading_choice, next_node });
			}

			std::pair<int, TreeNode*> SelectActionByRandom()
			{
				assert(GetCurrentNode()->HasAnyChild());

				size_t count = GetCurrentNode()->GetValidActionsCount();
				int idx = StaticConfigs::SelectionPhaseRandomActionPolicy::GetRandom((int)count);
				return GetCurrentNode()->GetNthValidAction((size_t)idx);
			}

			std::pair<int, TreeNode*> SelectActionByChoice(board::Board const& board)
			{
				assert(GetCurrentNode()->HasAnyValidAction());
				return StaticConfigs::SelectionPhaseSelectActionPolicy::GetChoice(
					policy::selection::ChoiceGetter(*GetCurrentNode()), board);
			}

		private:
			std::vector<TraversedNodeInfo> path_;

			std::vector<TraversedNodeInfo> saved_path_;
		};
	}
}