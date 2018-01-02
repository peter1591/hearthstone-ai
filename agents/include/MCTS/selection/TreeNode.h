#pragma once

#include <atomic>
#include <unordered_map>
#include <memory>

#include "MCTS/detail/TreeNodeBase.h"
#include "MCTS/selection/TreeNodeAddon.h"
#include "MCTS/selection/EdgeAddon.h"
#include "MCTS/selection/ChildNodeMap.h"
#include "Utils/SpinLocks.h"

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
				ChoiceIterator(engine::ActionChoices & choices, ChildNodeMap & children) :
					choices_(choices), children_(children),
					current_choice_(0), current_child_(nullptr)
				{}

				void Begin() { choices_.Begin(); }
				void StepNext() { choices_.StepNext(); }
				bool IsEnd() { return choices_.IsEnd(); }

				enum CheckResult {
					kForceSelectChoice,
					kNormalChoice
				};
				CheckResult Check() {
					current_choice_ = choices_.Get();
					assert(current_choice_ >= 0);

					current_child_ = children_.Get(current_choice_);
					return CheckChild(current_child_);
				}
				static CheckResult CheckChild(ChildType* child) {
					if (!child) return kForceSelectChoice; // not expanded before
					return kNormalChoice;
				}

				int GetChoice() const { return current_choice_; }
				mcts::selection::EdgeAddon const& GetAddon() const {
					return current_child_->GetEdgeAddon();
				}

			private:
				engine::ActionChoices & choices_;
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

			TreeNode() : children_mutex_(), children_(), addon_() {}

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
			int Select(engine::ActionType action_type, engine::ActionChoices choices, SelectCallback && select_callback)
			{
				// TODO: move to selection stage class

				// We can only acquire a shared lock,
				// since ChoiceIterator only calls children_.Get()
				std::shared_lock<Utils::SharedSpinLock> lock(children_mutex_);
				return select_callback.SelectChoice(
					ChoiceIterator(choices, children_)
				);
			}

			struct FollowStatus {
				bool just_expanded;
				EdgeAddon & edge_addon;
				TreeNode * node; // will be a non-nullptr
			};
			// Note: the choice should not be marked as redirect
			// If it's a redirect node, we should follow it by board view, not by choice
			FollowStatus FollowChoice(int choice)
			{
				// Optimize to only acquire a read lock if no need to create a new node

				std::shared_lock<Utils::SharedSpinLock> lock(children_mutex_);
				ChildType* child = children_.Get(choice);

				bool just_expanded = false;
				if (!child) {
					lock.unlock();

					{
						std::lock_guard<Utils::SharedSpinLock> write_lock(children_mutex_);
						child = children_.Get(choice);
						if (!child) {
							child = children_.CreateNewNode(choice, std::make_unique<TreeNode>());
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
				assert(child_node);
				return { just_expanded, child->GetEdgeAddon(), child_node };
			}

			EdgeAddon& MarkChoiceRedirect(int choice)
			{
				// Need a write lock since we modify child state
				std::lock_guard<Utils::SharedSpinLock> lock(children_mutex_);

				// the child node is not yet created
				// since we delay the node creation as late as possible
				ChildType* child = children_.Get(choice);
				if (!child) child = children_.CreateRedirectNode(choice);
				assert(child);
				return child->GetEdgeAddon();
			}

			EdgeAddon * GetEdgeAddon(int choice) {
				std::shared_lock<Utils::SharedSpinLock> lock(children_mutex_);

				ChildType * child = children_.Get(choice);
				if (!child) return nullptr;
				return &child->GetEdgeAddon();
			}

			EdgeAddon const* GetEdgeAddon(int choice) const {
				std::shared_lock<Utils::SharedSpinLock> lock(children_mutex_);

				ChildType const* child = children_.Get(choice);
				if (!child) return nullptr;
				return &child->GetEdgeAddon();
			}

			template <typename Functor>
			void ForEachChild(Functor&& functor) const {
				std::shared_lock<Utils::SharedSpinLock> lock(children_mutex_);

				children_.ForEach(std::forward<Functor>(functor));
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
			mutable Utils::SharedSpinLock children_mutex_;
			ChildNodeMap children_;

			TreeNodeAddon addon_;
		};
	}
}