#pragma once

#include "MCTS/selection/ChildNodeMap.h"

namespace mcts
{
	namespace selection
	{
		inline void ChildType::ClearNode() {
			if (!node_) return;
			if (is_redirect_node_) return;
			delete node_;
			node_ = nullptr;
		}
	}
}