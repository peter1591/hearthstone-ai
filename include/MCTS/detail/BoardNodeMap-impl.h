#pragma once

#include "MCTS/selection/TreeNode.h"
#include "MCTS/detail/BoardNodeMap.h"

namespace mcts
{
	namespace selection { class TreeNode; }

	namespace detail
	{
		inline BoardNodeMap::TreeNode* BoardNodeMap::GetOrCreateNode(board::Board const& board)
		{
			auto & item = GetMap()[board.CreateView()];
			if (!item) item.reset(new TreeNode());
			return item.get();
		}
	}
}