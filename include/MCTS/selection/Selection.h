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
					assert(parent->GetChild(edge) == child);

					bool do_remove = false;
					if (!node_removed) do_remove = true;
					else {
						if (child->HasUnExpandedAction()) {}
						else if (child->HasAnyChild()) {}
						else 
							do_remove = true;
					}

					if (!do_remove) return;
					
					parent->RemoveChild(edge);
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
				if (GetCurrentNode()->HasUnExpandedAction()) {
					*new_node = true;
					int next_action = GetCurrentNode()->GetNextActionToExpand();
					return { next_action, GetCurrentNode()->CreateChild(next_action) };
				}
				*new_node = false;

				if (!GetCurrentNode()->HasAnyChild()) {
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

				size_t count = GetCurrentNode()->GetChildrenCount();
				int idx = StaticConfigs::SelectionPhaseRandomActionPolicy::GetRandom((int)count);
				return GetCurrentNode()->GetNthChild((size_t)idx);
			}

			std::pair<int, TreeNode*> SelectActionByChoice(board::Board const& board)
			{
				assert(GetCurrentNode()->HasAnyChild());
				return StaticConfigs::SelectionPhaseSelectActionPolicy::GetChoice(
					policy::PolicyBase::ChoiceGetter(*GetCurrentNode()), board);
			}

		private:
			std::vector<TraversedNodeInfo> path_;

			std::vector<TraversedNodeInfo> saved_path_;
		};
	}
}