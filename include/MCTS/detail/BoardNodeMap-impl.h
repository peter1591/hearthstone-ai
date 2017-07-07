#pragma once

#include "MCTS/selection/TreeNode.h"
#include "MCTS/detail/BoardNodeMap.h"

namespace mcts
{
	namespace selection { class TreeNode; }

	namespace detail
	{
		inline BoardNodeMap::TreeNode* BoardNodeMap::GetOrCreateNode(board::Board const& board, bool * new_node_created)
		{
			if (new_node_created) *new_node_created = false;

			auto & item = GetMap()[board.CreateView()];
			if (!item) {
				item.reset(new TreeNode());
				if (new_node_created) *new_node_created = true;
			}
			
			return item.get();
		}
	}
}