#pragma once

#include "MCTS/selection/TreeNode.h"

namespace mcts
{
	namespace selection
	{
		struct TraversedNodeInfo {
			TreeNode* node_;
			int choice_; // choice lead to next node
			EdgeAddon * edge_addon_;

			TraversedNodeInfo(TreeNode* node, int choice, EdgeAddon * edge_addon) :
				node_(node), choice_(choice), edge_addon_(edge_addon)
			{}
		};
	}
}