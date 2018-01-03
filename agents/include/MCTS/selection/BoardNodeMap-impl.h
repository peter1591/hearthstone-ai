#pragma once

#include "MCTS/selection/BoardNodeMap.h"

#include "MCTS/selection/TreeNode.h"

namespace mcts
{
	namespace selection
	{
		inline TreeNode* BoardNodeMap::GetOrCreateNode(engine::view::Board const& board, bool * new_node_created)
		{
			auto board_view = board.CreateView();
			{
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				if (map_) {
					auto it = map_->find(board_view);
					if (it != map_->end()) {
						return it->second.get();
					}
				}
			}
			{
				std::lock_guard<Utils::SharedSpinLock> lock(mutex_);
				auto & item = LockedGetMap()[board_view];
				if (!item) {
					item.reset(new TreeNode());
					if (new_node_created) *new_node_created = true;
				}
				return item.get();
			}
		}
	}
}