#pragma once

#include <atomic>
#include <shared_mutex>
#include <unordered_map>
#include <memory>

#include "MCTS/Types.h"
#include "MCTS/detail/TreeNodeBase.h"
#include "MCTS/selection/TreeNodeAddon.h"
#include "MCTS/board/ActionChoices.h"
#include "MCTS/selection/EdgeAddon.h"
#include "MCTS/selection/ChildNodeMap.h"

namespace mcts
{
	namespace selection
	{
		class TreeNode
		{
		public:
			// Note: only calls children's Get(), so a caller can acquire a read-lock only
			class ChoiceIterator {
			public:
				ChoiceIterator(board::ActionChoices & choices, ChildNodeMap & children) :
					choices_(choices), children_(children),
					current_choice_(0), current_child_(nullptr)
				{}

				void Begin() { choices_.Begin(); }
				void StepNext() { choices_.StepNext(); }
				bool IsEnd() { return choices_.IsEnd(); }

				enum CheckResult {
					kForceSelectChoice,
					kNormalChoice,
					kInvalidChoice
				};
				CheckResult Check() {
					current_choice_ = choices_.Get();
					assert(current_choice_ >= 0);

					current_child_ = children_.Get(current_choice_);
					return CheckChild(current_child_);
				}
				static CheckResult CheckChild(ChildType* child) {
					if (!child) return kForceSelectChoice; // not expanded before
					if (child->IsInvalidNode()) return kInvalidChoice;
					return kNormalChoice;
				}

				int GetChoice() const { return current_choice_; }
				mcts::selection::EdgeAddon const& GetAddon() const {
					return current_child_->GetEdgeAddon();
				}

			private:
				board::ActionChoices & choices_;
				ChildNodeMap & children_;

				int current_choice_;
				ChildType * current_child_;
			};

		public:
			// Thread safety:
			//   the ChildNodeMap is not thread-safe
			//   the element ChildType within ChildNodeMap is also not thread-safe
			//   we use a read-write lock on ChildNodeMap
			//      if calling only Get(), we can acquire read lock only
			//      otherwise, need write lock
			//   Also, the element ChildType in ChildNodeMap will never be removed
			//   And thus, the EdgeAddon of ChildType will never be removed

			TreeNode() : 
				action_type_(ActionType::kInvalid),
				choices_type_(board::ActionChoices::kInvalid),
				children_mutex_(), children_(), addon_()
			{}

			// it is assumed we will never create a node at these special addresses
			static TreeNode* GetFirstWinNode() { return reinterpret_cast<TreeNode *>(0x1); }
			static TreeNode* GetSecondWinNode() { return reinterpret_cast<TreeNode *>((uint64_t)0x2); }

			bool IsFirstWinNode() const { return this == GetFirstWinNode(); }
			bool IsSecondWinNode() const { return this == GetSecondWinNode(); }
			bool IsWinNode() const { return IsFirstWinNode() || IsSecondWinNode(); }

			// select among specific choices
			// if any of the choices does not exist, return the edge to expand it
			//    A new node will be allocated
			// if all of the specific exist, use select_callback to select one of them
			//    Call select_callback.ReportChoicesCount(int count)
			//        Include invalid choices
			//    Call select_callback.AddChoice(int choice, EdgeAddon, TreeNode* node) for each choices
			//        If an action is marked invalid before, a nullptr is passed to the 'node' parameter
			//    Call select_callback.SelectChoice() -> TreeNode to get result
			// Return -1 if all choices are invalid.
			//    (or, the force_choice is invalid)
			template <typename SelectCallback>
			int Select(ActionType action_type, board::ActionChoices choices, SelectCallback && select_callback)
			{
				auto choices_type_loaded = choices_type_.load();
				if (choices_type_loaded == board::ActionChoices::kInvalid) {
					choices_type_loaded = choices_type_.exchange(choices.GetType());
				}
				if (choices_type_loaded != board::ActionChoices::kInvalid) {
					assert(choices_type_loaded == choices.GetType());
				}

				auto action_type_loaded = action_type_.load();
				if (action_type_loaded == ActionType::kInvalid) {
					action_type_loaded = action_type_.exchange(action_type.GetType());
				}
				if (action_type_loaded != ActionType::kInvalid) {
					assert(action_type_loaded == action_type.GetType());
				}

				// We can only acquire a shared lock,
				// since ChoiceIterator only calls children_.Get()
				std::shared_lock<std::shared_mutex> lock(children_mutex_);
				return select_callback.SelectChoice(
					ChoiceIterator(choices, children_)
				);
			}

			struct FollowStatus {
				bool just_expanded;
				EdgeAddon & edge_addon;
				TreeNode * node; // nullptr for an invalid choice
			};
			// Note: the choice should not be marked as redirect
			// If it's a redirect node, we should follow it by board view, not by choice
			FollowStatus FollowChoice(int choice)
			{
				// Optimize to only acquire a read lock if no need to create a new node

				std::shared_lock<std::shared_mutex> lock(children_mutex_);
				ChildType* child = children_.Get(choice);

				bool just_expanded = false;
				if (!child) {
					lock.unlock();

					{
						std::lock_guard<std::shared_mutex> write_lock(children_mutex_);
						child = children_.Get(choice);
						if (!child) {
							child = children_.Create(choice);
							child->SetNode(std::make_unique<TreeNode>());
							just_expanded = true;
						}
					}

					lock.lock();
					assert(children_.Get(choice)); // a child should never be removed by any thread
				}

				// Since a redirect node should only appear at the end of a main-action-sequence,
				// it should not be followed.
				assert(!child->IsRedirectNode());

				TreeNode* child_node = child->GetNode();
				return { just_expanded, child->GetEdgeAddon(), child_node };
			}

			EdgeAddon& MarkChoiceInvalid(int choice)
			{
				// Need a write lock since we modify child state
				std::lock_guard<std::shared_mutex> lock(children_mutex_);

				// the child node is not yet created
				// since we delay the node creation as late as possible
				ChildType* child = children_.Get(choice);

				if (child) {
					child->SetAsInvalidNode(); // mark invalid
				}
				else {
					child = children_.Create(choice); // create an invalid child node
				}
				return child->GetEdgeAddon();
			}

			EdgeAddon& MarkChoiceRedirect(int choice)
			{
				// Need a write lock since we modify child state
				std::lock_guard<std::shared_mutex> lock(children_mutex_);

				// the child node is not yet created
				// since we delay the node creation as late as possible
				ChildType* child = children_.Get(choice);
				if (!child) child = children_.Create(choice);
				assert(child);
				child->SetAsRedirectNode();
				return child->GetEdgeAddon();
			}

			EdgeAddon * GetEdgeAddon(int choice) {
				std::shared_lock<std::shared_mutex> lock(children_mutex_);

				ChildType * child = children_.Get(choice);
				if (!child) return nullptr;
				return &child->GetEdgeAddon();
			}

			EdgeAddon const* GetEdgeAddon(int choice) const {
				std::shared_lock<std::shared_mutex> lock(children_mutex_);

				ChildType const* child = children_.Get(choice);
				if (!child) return nullptr;
				return &child->GetEdgeAddon();
			}

			template <typename Functor>
			void ForEachChild(Functor&& functor) const {
				std::shared_lock<std::shared_mutex> lock(children_mutex_);

				children_.ForEach(std::forward<Functor>(functor));
			}

		public:
			ActionType GetActionType() const {
				return ActionType(action_type_.load());
			}

		public:
			TreeNodeAddon const& GetAddon() const { return addon_; }
			TreeNodeAddon & GetAddon() { return addon_; }

		public:
			// return nullptr if child does not exists, or its an invalid/redirect node
			TreeNode* GetChildNode(int choice) {
				auto item = children_.Get(choice);
				if (!item) return nullptr;
				return item->GetNode();
			}

		private:
			std::atomic<ActionType::Types> action_type_;
			std::atomic<board::ActionChoices::Type> choices_type_; // TODO: debug only

			mutable std::shared_mutex children_mutex_;
			ChildNodeMap children_;

			TreeNodeAddon addon_;
		};
	}
}