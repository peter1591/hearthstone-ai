#pragma once

#include <unordered_map>
#include <memory>

#include "MCTS/ActionType.h"

namespace mcts
{
	class TreeNode
	{
	public:
		TreeNode() : action_count_(0), action_type_(ActionType::kInvalid),
			valid_children_count_(0), wins(0), total(0)
		{}

		void FillActions(ActionType action_type, int action_count) {
			assert(action_count >= 0);

			if (action_count_ > 0) {
				// already filled, check if info is consistency
				assert(action_type_ == action_type);
				assert(action_count_ == action_count);
				return;
			}

			assert(children_.empty());
			assert(valid_children_count_ == 0);
			action_count_ = (size_t)action_count;
			action_type_ = action_type;
		}

		bool HasUnExpandedAction() const { return children_.size() < action_count_; }
		int GetNextActionToExpand() const {
			assert(HasUnExpandedAction());
			return (int)children_.size();
		}

		ActionType GetActionType() const { return action_type_; }
		int GetActionCount() const { return (int)action_count_; }

		bool HasAnyChild() const { return valid_children_count_ > 0; }

		template <typename Functor>
		void ForEachChild(Functor&& functor) const {
			for (int i = 0; i < children_.size(); ++i) {
				if (!children_[i]) continue; // removed
				functor(i, children_[i].get());
			}
		}

		TreeNode* GetChild(int action) {
			assert(action >= 0);
			assert(action < children_.size());
			TreeNode* child = children_[action].get();
			assert(child); // child should not be a nullptr (i.e., a removed child)
			return child;
		}

		TreeNode* CreateChild(int action) {
			assert(action == (int)children_.size()); // only possible to expand the next action
			TreeNode* new_node = new TreeNode();
			children_.push_back(std::unique_ptr<TreeNode>(new_node));
			++valid_children_count_;
			return new_node;
		}

		void RemoveChild(int action) {
			assert(action >= 0);
			assert(action < GetActionCount());
			assert(action < children_.size());
			assert(children_[action]);
			children_[action].release();
			--valid_children_count_;
		}

		void ReportResult(bool win) {
			if (win) ++wins;
			++total;
		}

	private:
		ActionType action_type_; // TODO: actually this is debug only to check consistency of game engine
		size_t action_count_;
		size_t valid_children_count_;
		std::vector<std::unique_ptr<TreeNode>> children_;

		int wins;
		int total;
	};

	class Tree
	{
	public:
		TreeNode* GetRootNode() { return &root_; }

	private:
		TreeNode root_;
	};
}