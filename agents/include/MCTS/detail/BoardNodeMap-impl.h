#pragma once

#include "MCTS/selection/TreeNode.h"
#include "MCTS/detail/BoardNodeMap.h"

namespace mcts
{
	namespace detail
	{
		inline BoardNodeMap::TreeNode* BoardNodeMap::GetOrCreateNode(engine::view::Board const& board, bool * new_node_created)
		{
			std::lock_guard<Utils::SharedSpinLock> lock(mutex_);

			auto & item = GetMap()[board.CreateView()];
			if (!item) {
				item.reset(new TreeNode());
				if (new_node_created) *new_node_created = true;
			}
			
			return item.get();
		}
	}
}