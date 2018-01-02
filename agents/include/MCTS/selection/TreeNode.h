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
			// Thread safety:
			//   the ChildNodeMap is not thread-safe
			//   the element ChildType within ChildNodeMap is also not thread-safe
			//   we use a read-write lock on ChildNodeMap
			//      if calling only Get(), we can acquire read lock only
			//      otherwise, need write lock
			//   Also, the element ChildType in ChildNodeMap will never be removed
			//   And thus, the EdgeAddon of ChildType will never be removed

			TreeNode() : children_mutex_(), children_(), addon_() {}

			template <class Functor>
			auto GetChildren(Functor&& functor) const {
				std::shared_lock<Utils::SharedSpinLock> lock(children_mutex_);
				return functor(children_);
			}
			template <class Functor>
			auto GetMutableChildren(Functor&& functor) {
				std::shared_lock<Utils::SharedSpinLock> lock(children_mutex_);
				return functor(children_);
			}

			// @return  A boolean indicating if a new node is created; then the child data
			template <class CreateChildFunctor>
			std::pair<bool, ChildType *> GetOrCreateChild(int choice, CreateChildFunctor&& create_child_functor) {
				// Optimize to only acquire a read lock if no need to create a new node
				{
					std::shared_lock<Utils::SharedSpinLock> lock(children_mutex_);
					ChildType* child = children_.Get(choice);
					if (child) return { false, child };
				}

				{
					std::lock_guard<Utils::SharedSpinLock> write_lock(children_mutex_);
					ChildType* child = children_.Get(choice);
					if (child) return { false, child };
					child = create_child_functor(children_);
					return { true, child };
				}
			}

		public:
			EdgeAddon const* GetEdgeAddon(int choice) const {
				return GetChildren([choice](ChildNodeMap const& children) -> EdgeAddon const* {
					ChildType const* child = children.Get(choice);
					if (!child) return nullptr;
					return &child->GetEdgeAddon();
				});
			}
			EdgeAddon * GetMutableEdgeAddon(int choice) {
				return GetMutableChildren([choice](ChildNodeMap & children) -> EdgeAddon * {
					ChildType * child = children.Get(choice);
					if (!child) return nullptr;
					return &child->GetEdgeAddon();
				});
			}

			template <typename Functor>
			void ForEachChild(Functor&& functor) const {
				return GetChildren([&](ChildNodeMap const& children) {
					children.ForEach(std::forward<Functor>(functor));
				});
			}

		public:
			TreeNodeAddon const& GetAddon() const { return addon_; }
			TreeNodeAddon & GetAddon() { return addon_; }

		private:
			mutable Utils::SharedSpinLock children_mutex_;
			ChildNodeMap children_;

			TreeNodeAddon addon_;
		};
	}
}