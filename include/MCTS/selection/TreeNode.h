#pragma once

#include <unordered_map>
#include <memory>

#include "MCTS/ActionType.h"
#include "MCTS/detail/TreeNodeBase.h"
#include "MCTS/selection/TreeNodeAddon.h"

namespace mcts
{
	namespace selection
	{
		// Action: Use the index of all children
		// Remove a child: Just mark the children as invalid
		class TreeNode : private detail::TreeNodeBase<TreeNode>
		{
		public:
			TreeNode() : action_count_(0), action_type_(ActionType::kInvalid), wins_(0), total_(0) {}

			ActionType GetActionType() const { return action_type_; }
			int GetActionCount() const { return (int)action_count_; }

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

			bool HasUnExpandedAction() const {
				return TreeNodeBase::GetChildrenCount() < action_count_;
			}
			int GetNextActionToExpand() const {
				assert(HasUnExpandedAction());
				return (int)TreeNodeBase::GetChildrenCount();
			}

			bool HasAnyChild() const { return TreeNodeBase::HasValidChild(); }
			size_t GetChildrenCount() const { return TreeNodeBase::GetValidChildrenCount(); }

			template <typename Functor>
			void ForEachChild(Functor&& functor) const {
				return TreeNodeBase::ForEachValidChild(std::forward<Functor>(functor));
			}

			std::pair<int, TreeNode*> GetNthChild(size_t idx) const {
				auto ret = TreeNodeBase::GetValidChild(idx);
				return { (int)ret.first, ret.second };
			}

			TreeNode* GetChild(int action) { return TreeNodeBase::GetChild((size_t)action); }

			TreeNode* CreateChild(int action) {
				assert(action == (int)TreeNodeBase::GetChildrenCount()); // only possible to expand the next action
				return TreeNodeBase::PushBackValidChild();
			}

			void RemoveChild(int action) {
				assert(action >= 0);
				assert((size_t)action < TreeNodeBase::GetChildrenCount());
				return TreeNodeBase::MarkInvalid((size_t)action);
			}

			void ReportResult(bool win) {
				if (win) ++wins_;
				++total_;
			}

		public:
			TreeNodeAddon const& GetAddon() const { return addon_; }
			TreeNodeAddon & GetAddon() { return addon_; }

		private:
			ActionType action_type_; // TODO: actually this is debug only to check consistency of game engine
			size_t action_count_;
			int wins_;
			int total_;

			TreeNodeAddon addon_;
		};
	}
}