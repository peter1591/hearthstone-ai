#pragma once

#include <unordered_map>
#include <memory>
#include "MCTS/selection/EdgeAddon.h"

namespace mcts
{
	namespace selection
	{
		class TreeNode;

		struct ChildType
		{
			EdgeAddon edge_addon;
			std::unique_ptr<TreeNode> node;
		};

		class ChildNodeMap
		{
		public:
			// Hash table is not used here, since
			//   1. we don't know the total choices in advance
			//   2. the key is 'choice', which might be card id for choose-one action
			using ChildMapType = std::unordered_map<int, ChildType>;

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