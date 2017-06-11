#pragma once

#include <assert.h>
#include "MCTS/Tree.h"

namespace mcts
{
	namespace stages
	{
		class Selection
		{
		public:
			struct TraversedNodeInfo {
				int leading_choice; // choice to lead to this node
				TreeNode* node;
			};

		public:
			void StartEpisode()
			{
				path_.clear();
				StepNext(-1, tree_.GetRootNode());
			}

			// @return >= 0 for the chosen action; < 0 if no valid action
			int GetAction(ActionType action_type, int choices, bool * created_new_node)
			{
				ReportActionInfo(action_type, choices);
				
				auto next_info = SelectAction(action_type, choices, created_new_node);
				
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

			template <typename Functor>
			void ForEachTraversedPath(Functor&& functor) {
				for (auto const& item : path_) {
					functor(item.leading_choice, item.node);
				}
			}

		private:
			TreeNode* GetCurrentNode() const { return path_.back().node; }

			void ReportActionInfo(ActionType action_type, int choices) {
				GetCurrentNode()->FillActions(action_type, choices);
			}

			std::pair<int, TreeNode*> SelectAction(ActionType action_type, int choices, bool * new_node) {
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
				else return SelectActionByChoice();
			}

			void StepNext(int leading_choice, TreeNode* next_node)
			{
				path_.push_back({ leading_choice, next_node });
			}

			std::pair<int, TreeNode*> SelectActionByRandom()
			{
				// TODO: a quicker way random access the tree_node_->GetChildren()
				assert(GetCurrentNode()->HasAnyChild());
				std::vector<std::pair<int, TreeNode*>> valid_choices;
				GetCurrentNode()->ForEachChild([&](int action, TreeNode* child) {
					valid_choices.push_back({ action, child });
				});
				int idx = std::rand() % valid_choices.size(); // TODO: use more stronger random generator?
				return valid_choices[idx];
			}

			std::pair<int, TreeNode*> SelectActionByChoice()
			{
				assert(GetCurrentNode()->HasAnyChild());
				std::vector<std::pair<int, TreeNode*>> valid_choices;
				GetCurrentNode()->ForEachChild([&](int action, TreeNode* child) {
					valid_choices.push_back({ action, child });
				});
				int idx = std::rand() % valid_choices.size(); // TODO: use more stronger random generator?
				return valid_choices[idx];
			}

		private:
			Tree tree_;

			std::vector<TraversedNodeInfo> path_;
		};
	}
}