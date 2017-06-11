#pragma once

#include <assert.h>
#include "MCTS/Tree.h"

namespace mcts
{
	namespace stages
	{
		class Selection
		{
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
				StepNext(tree_.GetRootNode());
			}

			// @return >= 0 for the chosen action; < 0 if no valid action
			int GetAction(ActionType action_type, int choices, bool * created_new_node)
			{
				ReportActionInfo(action_type, choices);
				int choice = SelectAction(action_type, choices, created_new_node);
				if (choice < 0) {
					last_node_.Set(nullptr, nullptr, -1);
					return -1;
				}

				TreeNode* parent_node = tree_node_;
				TreeNode* next_node = parent_node->GetOrCreateChild(choice);
				
				StepNext(next_node);
				last_node_.Set(parent_node, next_node, choice);

				return choice;
			}

			void ReportInvalidAction() {
				// We modify the tree structure, so next time this invalid node will not be chosen
				if (!last_node_.GetChild()) return;
				assert(tree_node_ == last_node_.GetChild());
				last_node_.RemoveNode();
			}

			std::vector<TreeNode*> const& GetTraversedPath() const { return path_; }

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

			void StepNext(TreeNode* next_node)
			{
				tree_node_ = next_node;
				assert(tree_node_);
				path_.push_back(tree_node_);
			}

			int SelectActionByRandom(int choices)
			{
				return std::rand() % choices; // TODO: use more stronger random generator?
			}

			int SelectActionByChoice(int choices)
			{
				assert(!tree_node_->GetChildren().empty());
				return tree_node_->GetChildren().begin()->first;
			}

		private:
			Tree tree_;
			TreeNode* tree_node_;

			LastNodeInfo last_node_;
			std::vector<TreeNode*> path_;
		};
	}
}