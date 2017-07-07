#pragma once

#include <unordered_map>
#include <memory>
#include "MCTS/selection/EdgeAddon.h"

namespace mcts
{
	namespace selection
	{
		class TreeNode;

		class ChildType
		{
		public:
			ChildType() : edge_addon_(), is_redirect_node_(false), node_(nullptr) {}
			ChildType(ChildType const&) = delete;
			ChildType & operator=(ChildType const&) = delete;

			EdgeAddon & GetEdgeAddon() { return edge_addon_; }
			
			void SetNode(TreeNode * node) {
				ClearNode();
				is_redirect_node_ = false;
				node_ = node;
			}

			void SetAsRedirectNode(TreeNode * node) {
				ClearNode();
				is_redirect_node_ = true;
				node_ = node;
			}

			bool IsRedirectNode() const { return is_redirect_node_; }
			TreeNode * GetNode() const { return node_; }

		private:
			void ClearNode();

		private:
			EdgeAddon edge_addon_;
			bool is_redirect_node_;
			TreeNode * node_;
		};

		class ChildNodeMap
		{
		public:
			// Hash table is not used here, since
			//   1. we don't know the total choices in advance
			//   2. the key is 'choice', which might be card id for choose-one action
			using ChildMapType = std::unordered_map<int, ChildType>;

			ChildNodeMap() : map_() {}

			ChildType* Get(int choice) {
				auto it = map_.find(choice);
				if (it == map_.end()) return nullptr;
				return &(it->second);
			}

			ChildType* Create(int choice) {
				assert(map_.find(choice) == map_.end());
				return &map_[choice];
			}

		private:
			ChildMapType map_;
		};
	}
}