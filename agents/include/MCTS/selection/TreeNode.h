#pragma once

#include <atomic>
#include <unordered_map>
#include <memory>

#include "MCTS/selection/TreeNodeAddon.h"
#include "MCTS/selection/EdgeAddon.h"
#include "Utils/SpinLocks.h"

namespace mcts
{
	namespace selection
	{
		struct TreeNode;

		// Thread safe
		class ChildNodeMap
		{
		private:
			// Note: this is not thread safe. do not expose it to outside
			struct ChildType
			{
			public:
				ChildType() : edge_addon_(), node_() {}

				EdgeAddon edge_addon_; // must be thread safe
				std::unique_ptr<TreeNode> node_;
			};

		private:
			// @return  A boolean indicating if a new node is created; then the child data
			template <class CreateFunctor>
			std::tuple<bool, EdgeAddon*, TreeNode*> GetOrCreate(int choice, CreateFunctor&& create_child_functor) {
				// Optimize to only acquire a read lock if no need to create a new node
				{
					std::shared_lock<Utils::SharedSpinLock> lock(map_mutex_);
					auto it = map_.find(choice);
					if (it != map_.end()) return { false, &it->second.edge_addon_, it->second.node_.get() };
				}

				{
					std::lock_guard<Utils::SharedSpinLock> write_lock(map_mutex_);
					auto it = map_.find(choice);
					if (it != map_.end()) return { false, &it->second.edge_addon_, it->second.node_.get() };
					auto & child = map_[choice];
					create_child_functor(child);
					return { true, &child.edge_addon_, child.node_.get() };
				}
			}

		public:
			ChildNodeMap() : map_mutex_(), map_() {}

			bool HasChild(int choice) const {
				std::shared_lock<Utils::SharedSpinLock> lock(map_mutex_);
				return (map_.find(choice) != map_.end());
			}

			// The EdgeAddon is exposed. It should be thread-safe by itself
			std::pair<EdgeAddon const*, TreeNode*> Get(int choice) const {
				std::shared_lock<Utils::SharedSpinLock> lock(map_mutex_);
				auto it = map_.find(choice);
				if (it == map_.end()) return { nullptr, nullptr };
				else return { &it->second.edge_addon_, it->second.node_.get() };
			}

			template <typename Functor>
			void ForEach(Functor&& functor) const {
				std::shared_lock<Utils::SharedSpinLock> lock(map_mutex_);
				for (auto const& kv : map_) {
					if (!functor(kv.first, &kv.second.edge_addon_, kv.second.node_.get())) return;
				}
			}

			template <typename Functor>
			void ForEach(Functor&& functor) {
				std::shared_lock<Utils::SharedSpinLock> lock(map_mutex_);
				for (auto & kv : map_) {
					if (!functor(kv.first, &kv.second.edge_addon_, kv.second.node_.get())) return;
				}
			}

		public:
			EdgeAddon const* GetEdgeAddon(int choice) const {
				return Get(choice).first;
			}

			std::tuple<bool, EdgeAddon*, TreeNode*> GetOrCreateNewNode(int choice, std::unique_ptr<TreeNode> node) {
				return GetOrCreate(choice, [&](ChildType & child) {
					child.node_ = std::move(node);
				});
			}
			std::tuple<bool, EdgeAddon*, TreeNode*> GetOrCreatRedirectNode(int choice) {
				return GetOrCreate(choice, [](ChildType & child) {
					assert(child.node_.get() == nullptr);
				});
			}

		private:
			mutable Utils::SharedSpinLock map_mutex_;

			// Hash table is used here, since
			//   1. we don't know the total choices in advance
			//   2. the key is 'choice', which might be card id for choose-one action
			// TODO: maybe use std::vector is enough; or use linked-list for lock-free algorithms
			std::unordered_map<int, ChildType> map_;
		};

		// Thread safe
		struct TreeNode
		{
			TreeNode() : children_(), addon_() {}

			ChildNodeMap children_; // must be thread safe
			TreeNodeAddon addon_; // must be thread safe
		};
	}
}
