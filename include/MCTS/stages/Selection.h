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

		private:
			class LastNodeInfo {
			public:
				LastNodeInfo() : parent_(nullptr), action_(0), child_(nullptr) {}
				void Set(TreeNode* parent, TreeNode* child, int action) {
					parent_ = parent;
					child_ = child;
					action_ = action;
				}

				TreeNode* GetParent() const { return parent_; }
				TreeNode* GetChild() const { return child_; }
				int GetAction() const { return action_; }

				void RemoveNode() {
					assert(parent_->GetChild(action_) == child_);
					parent_->RemoveChild(action_);
				}

			private:
				TreeNode* parent_;
				int action_;
				TreeNode* child_;
			};

		public:
			Selection() : tree_node_(nullptr) {}

			void StartEpisode()
			{
				path_.clear();
				StepNext(-1, tree_.GetRootNode());
			}

			// @return >= 0 for the chosen action; < 0 if no valid action
			int GetAction(ActionType action_type, int choices, bool * created_new_node)
			{
				ReportActionInfo(action_type, choices);
				int choice = SelectAction(action_type, choices, created_new_node);
				if (choice < 0) {
					// if a (parent) node is with no valid child node
					// we should have deleted the (parent) node when the child node
					// got removed.
					assert(false);
					return -1;
				}

				TreeNode* parent_node = tree_node_;
				TreeNode* next_node = parent_node->GetOrCreateChild(choice);

				StepNext(choice, next_node);
				last_node_.Set(parent_node, next_node, choice);

				return choice;
			}

			void ReportInvalidAction() {
				assert(last_node_.GetChild());
				
				assert(tree_node_ == last_node_.GetChild());
				assert(!path_.empty());
				assert(path_.back().node == last_node_.GetChild());
				
				bool node_removed = false;
				auto op = [node_removed](TreeNode* parent, int edge, TreeNode* child) mutable {
					assert(parent->GetChild(edge) == child);

					bool do_remove = false;
					if (!node_removed) do_remove = true;
					else {
						if (child->HasUnExpandedAction()) {}
						else if (!child->GetChildren().empty()) {}
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
			void ReportActionInfo(ActionType action_type, int choices) {
				if (tree_node_->NeedFillActions()) {
					tree_node_->FillActions(action_type, choices);
				}
				assert(tree_node_->CheckFilledActions(action_type, choices));
			}

			int SelectAction(ActionType action_type, int choices, bool * new_node) {
				// Check if current tree node has un-expanded action
				//   If yes, choose that action
				if (tree_node_->HasUnExpandedAction()) {
					*new_node = true;
					return tree_node_->ExpandAction();
				}
				*new_node = false;

				if (tree_node_->GetChildren().empty()) {
					// no valid action from this node (all actions are removed since they yields an invalid state)
					return -1;
				}

				if (action_type.IsChosenRandomly()) return SelectActionByRandom(choices);
				else return SelectActionByChoice(choices);
			}

			void StepNext(int leading_choice, TreeNode* next_node)
			{
				tree_node_ = next_node;
				assert(tree_node_);
				path_.push_back({ leading_choice, tree_node_ });
			}

			int SelectActionByRandom(int choices)
			{
				// TODO: we should only enumerate over valid choice
				// TODO: a quicker way random access the tree_node_->GetChildren()
				return std::rand() % choices; // TODO: use more stronger random generator?
			}

			int SelectActionByChoice(int choices)
			{
				assert(!tree_node_->GetChildren().empty());
				std::vector<int> valid_choices;
				for (auto const& valid_child : tree_node_->GetChildren()) {
					valid_choices.push_back(valid_child.first);
				}
				int idx = std::rand() % valid_choices.size();
				return valid_choices[idx];
			}

		private:
			Tree tree_;
			TreeNode* tree_node_; // TODO: remove this, just use path_.back().node

			LastNodeInfo last_node_;

			std::vector<TraversedNodeInfo> path_;
		};
	}
}