#pragma once

#include <memory>

namespace mcts
{
	class TreeNode
	{
	public:
		TreeNode() : action_count_(0), is_random_(false) {}

		bool NeedFillActions() const { return action_count_ <= 0; }
		void FillActions(int action_count, bool random) {
			assert(action_count >= 0);
			action_count_ = (size_t)action_count;
			is_random_ = random;
			children_.clear();
		}

		bool HasUnExpandedAction() const { return children_.size() < action_count_; }
		int ExpandAction() {
			int next_action = (int)children_.size();
			children_.push_back(std::make_unique<TreeNode>());
			return next_action;
		}

		int GetActionCount() const { return (int)action_count_; }
		bool GetActionIsRandom() const { return is_random_; }

		TreeNode* GetChild(int action)
		{
			assert(action >= 0);
			assert(action < children_.size());
			return children_[action].get();
		}

	private:
		size_t action_count_;
		bool is_random_;
		std::vector<std::unique_ptr<TreeNode>> children_;
	};

	class Tree
	{
	public:
		TreeNode* GetRootNode() { return &root_; }

	private:
		TreeNode root_;
	};
}