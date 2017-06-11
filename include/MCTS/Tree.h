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
			assert(valid_children_idx_map_.empty());
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

		bool HasAnyChild() const { return !valid_children_idx_map_.empty(); }
		size_t GetChildrenCount() const { return valid_children_idx_map_.size(); }

		template <typename Functor>
		void ForEachChild(Functor&& functor) const {
			for (int i = 0; i < children_.size(); ++i) {
				if (!children_[i]) continue; // removed
				functor(i, children_[i].get());
			}
		}

		std::pair<int, TreeNode*> GetNthChild(size_t idx) const {
			assert(idx < valid_children_idx_map_.size());
			size_t child_idx = valid_children_idx_map_[idx];
			assert(child_idx < children_.size());
			assert(children_[child_idx]); // child should be valid (not removed)
			return { (int)child_idx, children_[child_idx].get() };
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
			valid_children_idx_map_.push_back(children_.size());
			children_.push_back(std::unique_ptr<TreeNode>(new_node));
			return new_node;
		}

		void RemoveChild(int action) {
			assert(action >= 0);
			assert(action < GetActionCount());
			assert(action < children_.size());
			assert(children_[action]);

			assert([&]() {
				bool found = false;
				for (auto it = valid_children_idx_map_.begin(); it != valid_children_idx_map_.end(); ++it)
				{
					if (*it == action) {
						found = true;
						break;
					}
				}
				assert(found);
				return true;
			}());

			children_[action].release();

			// A O(N) algorithm for removal, but the good side is
			// we can have a O(1) for selection (which should be most of the cases)
			for (auto it = valid_children_idx_map_.begin(); it != valid_children_idx_map_.end();)
			{
				if (*it == action) {
					it = valid_children_idx_map_.erase(it);
					break;
				}
				++it;
			}

			// debug check
			assert([&](){
				for (auto idx : valid_children_idx_map_) {
					assert(children_[idx]);
				}
				return true;
			}());
		}

		void ReportResult(bool win) {
			if (win) ++wins;
			++total;
		}

	private:
		ActionType action_type_; // TODO: actually this is debug only to check consistency of game engine
		size_t action_count_;
		std::vector<std::unique_ptr<TreeNode>> children_;

		// map the 'index to valid children' to the 'index to all children'
		std::vector<size_t> valid_children_idx_map_;

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