#pragma once

#include <unordered_map>
#include <memory>

#include "MCTS/Types.h"
#include "MCTS/detail/TreeNodeBase.h"
#include "MCTS/selection/TreeNodeAddon.h"

namespace mcts
{
	namespace selection
	{
		// Maintain two groups of actions
		//   1. Unexpanded actions
		//   2. Valid actions
		// Note: FillActions() should always be the first call
		class TreeNode : private detail::TreeNodeBase<TreeNode>
		{
		public:
			TreeNode() : action_count_(0), action_type_(ActionType::kInvalid) {}

			ActionType GetActionType() const { return action_type_; }
			int GetActionCount() const { return (int)action_count_; }

			// TODO: this might also be called for random actions
			// so 'action_count' might be large to choose a random from a wide range
			void FillActions(ActionType action_type, int action_count) {
				assert(action_count >= 0);

				if (action_count_ > 0) {
					// already filled, check if info is consistency
					assert(action_type_ == action_type);
					assert(action_count_ == action_count);
					return;
				}

				assert(!TreeNodeBase::HasChild());
				assert(!TreeNodeBase::HasValidChild());
				action_count_ = (size_t)action_count;
				action_type_ = action_type;
			}

			bool ExpandDone() const {
				return TreeNodeBase::GetChildrenCount() >= action_count_;
			}
			int GetNextActionToExpand() const {
				int idx = (int)TreeNodeBase::GetChildrenCount();
				while (idx < action_count_) {
					// TODO: check if this action is marked as invalid before
					return idx;
				}
				return -1;
			}

			bool HasAnyValidAction() const { return TreeNodeBase::HasValidChild(); }
			size_t GetValidActionsCount() const { return TreeNodeBase::GetValidChildrenCount(); }

			template <typename Functor>
			void ForEachValidAction(Functor&& functor) const {
				return TreeNodeBase::ForEachValidChild(std::forward<Functor>(functor));
			}

			std::pair<int, TreeNode*> GetNthValidAction(size_t idx) const {
				auto ret = TreeNodeBase::GetValidChild(idx);
				return { (int)ret.first, ret.second };
			}

			TreeNode* CreateAction(int action) {
				assert(action == (int)TreeNodeBase::GetChildrenCount()); // only possible to expand the next action
				return TreeNodeBase::PushBackValidChild();
			}
			TreeNode* GetAction(int action) { return TreeNodeBase::GetChild((size_t)action); }

			void MarkInvalidAction(int action) {
				assert(action >= 0);
				// TODO: mark invalid for unexpanded actyions
				assert((size_t)action < TreeNodeBase::GetChildrenCount());
				return TreeNodeBase::MarkInvalid((size_t)action);
			}

		public:
			TreeNodeAddon const& GetAddon() const { return addon_; }
			TreeNodeAddon & GetAddon() { return addon_; }

		private:
			ActionType action_type_; // TODO: actually this is debug only to check consistency of game engine
			size_t action_count_;
			TreeNodeAddon addon_;
		};
	}
}