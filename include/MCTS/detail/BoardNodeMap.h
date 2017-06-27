#pragma once

#include <memory>
#include <unordered_map>
#include "MCTS/board/Board.h"

namespace mcts
{
	namespace selection { class TreeNode; }

	namespace detail
	{
		class BoardNodeMap
		{
		private:
			using TreeNode = mcts::selection::TreeNode;
			using MapType = std::unordered_map<board::BoardView, std::unique_ptr<TreeNode>>;

		public:
			TreeNode* GetOrCreateNode(board::Board const& board);

		private:
			MapType & GetMap()
			{
				if (!map_) map_.reset(new MapType());
				return *map_.get();
			}

		private:
			std::unique_ptr<MapType> map_;
		};
	}
}