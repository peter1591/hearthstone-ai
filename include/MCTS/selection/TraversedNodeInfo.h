#pragma once

#include "MCTS/selection/TreeNode.h"

namespace mcts
{
	namespace selection
	{
		struct TraversedNodeInfo {
			int leading_choice; // choice to lead to this node
			EdgeAddon * edge_addon;
			TreeNode* node;
		};
	}
}