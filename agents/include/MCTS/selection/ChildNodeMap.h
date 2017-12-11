#pragma once

#include <unordered_map>
#include <memory>
#include "MCTS/selection/EdgeAddon.h"

namespace mcts
{
	namespace selection
	{
		class TreeNode;

		// Note: after a new node is created, the node should not be deleted
		// Since another thread might investigating that node (or its children)
		// Thread safety:
		//    Can be read from several threads concurrently
		//    Can only be write from one thread
		class ChildType
		{
		private:
			enum Type {
				kNormal,
				kRedirect
			};

		public:
			ChildType() : edge_addon_(), type_(kNormal), node_() {}

			ChildType(ChildType const&) = delete;
			ChildType & operator=(ChildType const&) = delete;

			EdgeAddon & GetEdgeAddon() { return edge_addon_; }
			EdgeAddon const& GetEdgeAddon() const { return edge_addon_; }
			
			void SetNode(std::unique_ptr<TreeNode> node) {
				assert(!node_);
				assert(type_ == kNormal);

				assert(node);
				node_ = std::move(node);
			}

			void SetAsRedirectNode() {
				assert(!node_);
				assert(type_ == kNormal || type_ == kRedirect);

				type_ = kRedirect;
			}

			bool IsRedirectNode() const { return type_ == kRedirect; }

			// return nullptr for redirect/invalid nodes
			TreeNode * GetNode() const {
				if (type_ == kNormal) return node_.get();
				else return nullptr;
			}

		private:
			EdgeAddon edge_addon_;
			Type type_; // TODO: atomic?
			std::unique_ptr<TreeNode> node_; // TODO: atomic?
		};

		// Thread safety:
		//    Multiple threads can read (including both Get()) concurrently.
		//    Should be modified by only one thread
		class ChildNodeMap
		{
		public:
			// Hash table is used here, since
			//   1. we don't know the total choices in advance
			//   2. the key is 'choice', which might be card id for choose-one action
			using ChildMapType = std::unordered_map<int, ChildType>;

			ChildNodeMap() : map_() {}

			ChildType * Get(int choice) {
				auto it = map_.find(choice);
				if (it == map_.end()) return nullptr;
				return &(it->second);
			}

			ChildType const* Get(int choice) const {
				auto it = map_.find(choice);
				if (it == map_.end()) return nullptr;
				return &(it->second);
			}

			// Once a child is created, it should not be destroyed
			// Since it might still be used in another thread
			ChildType* CreateNewNode(int choice, std::unique_ptr<TreeNode> node) {
				assert(map_.find(choice) == map_.end());
				ChildType & child = map_[choice];
				child.SetNode(std::move(node));
				return &child;
			}

			ChildType* CreateRedirectNode(int choice) {
				assert(map_.find(choice) == map_.end());
				ChildType & child = map_[choice];
				child.SetAsRedirectNode();
				return &child;
			}

			template <typename Functor>
			void ForEach(Functor&& functor) const {
				for (auto const& kv : map_) {
					if (!functor(kv.first, kv.second)) return;
				}
			}

		private:
			ChildMapType map_;
		};
	}
}