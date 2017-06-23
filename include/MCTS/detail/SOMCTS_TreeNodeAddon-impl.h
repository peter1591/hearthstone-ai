#pragma once

#include "MCTS/selection/TreeNode.h"
#include "MCTS/detail/SOMCTS_TreeNodeAddon.h"

namespace mcts
{
	namespace selection { class TreeNode; }

	namespace detail
	{
		inline SOMCTS_TreeNodeAddon::TreeNode* SOMCTS_TreeNodeAddon::GetOrCreateNode(board::Board const& board)
		{
			auto & item = GetMap()[board.CreateView()];
			if (!item) item.reset(new TreeNode());
			return item.get();
		}
	}
}