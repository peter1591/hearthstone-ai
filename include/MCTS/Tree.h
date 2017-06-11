#pragma once

#include <unordered_map>
#include <memory>

#include "MCTS/ActionType.h"

namespace mcts
{
	class TreeNode
	{
	public:
		TreeNode() : action_count_(0), action_type_(ActionType::kInvalid), next_unexpanded_action_(-1),
			wins(0), total(0)
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
			action_count_ = (size_t)action_count;
			action_type_ = action_type;
			next_unexpanded_action_ = 0;
		}

		bool HasUnExpandedAction() const { return next_unexpanded_action_ < action_count_; }
		int ExpandAction() { return next_unexpanded_action_++; }

		ActionType GetActionType() const { return action_type_; }
		int GetActionCount() const { return (int)action_count_; }

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
		ActionType action_type_; // TODO: actually this is debug only to check consistency of game engine
		size_t action_count_;
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