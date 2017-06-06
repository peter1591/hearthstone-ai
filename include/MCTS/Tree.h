#pragma once

#include <unordered_map>
#include <memory>

namespace mcts
{
	class TreeNode
	{
	public:
		TreeNode() : action_count_(0), is_random_(false), next_unexpanded_action_(-1),
			wins(0), total(0)
		{}

		bool NeedFillActions() const { return action_count_ <= 0; }
		void FillActions(int action_count, bool random) {
			assert(action_count >= 0);
			assert(children_.empty());
			action_count_ = (size_t)action_count;
			is_random_ = random;
			next_unexpanded_action_ = 0;
		}

		bool HasUnExpandedAction() const { return next_unexpanded_action_ < action_count_; }
		int ExpandAction() { return next_unexpanded_action_++; }

		int GetActionCount() const { return (int)action_count_; }
		bool GetActionIsRandom() const { return is_random_; }

		auto const& GetChildren() const { return children_; }

		TreeNode* GetChild(int action) {
			assert(action >= 0);
			assert(action < GetActionCount());
			return children_[action].get();
		}

		TreeNode* GetOrCreateChild(int action)
		{
			assert(action >= 0);
			assert(action < GetActionCount());
			
			auto& child = children_[action];
			if (!child.get()) child.reset(new TreeNode());
			return child.get();
		}

		void RemoveChild(int action) {
			assert(action >= 0);
			assert(action < GetActionCount());
			children_.erase(action);
		}

		void ReportResult(bool win) {
			if (win) ++wins;
			++total;
		}

	private:
		size_t action_count_;
		bool is_random_;
		int next_unexpanded_action_;
		std::unordered_map<int, std::unique_ptr<TreeNode>> children_;

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