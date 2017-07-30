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
				kRedirect,
				kInvalid
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

			void SetAsInvalidNode() {
				assert(!node_);
				assert(type_ == kNormal || type_ == kInvalid);

				type_ = kInvalid;
			}

			bool IsRedirectNode() const { return type_ == kRedirect; }
			bool IsInvalidNode() const { return type_ == kInvalid; }

			// return nullptr for redirect/invalid nodes
			TreeNode * GetNode() const {
				if (type_ == kNormal) return node_.get();
				else return nullptr;
			}

		private:
			EdgeAddon edge_addon_;
			Type type_;
			std::unique_ptr<TreeNode> node_;
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
			ChildType* Create(int choice) {
				assert(map_.find(choice) == map_.end());
				return &map_[choice];
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