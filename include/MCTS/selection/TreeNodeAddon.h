#pragma once

#include "MCTS/detail/BoardNodeMap.h"

namespace mcts
{
	namespace selection
	{
		struct TreeNodeStatistic
		{
			int credit;
			int total;

			TreeNodeStatistic() : credit(0), total(0) {}
		};

		// Add abilities to tree node to use in SO-MCTS
		// Note: this will lived in *every* tree node, so careful about memory footprints
		struct TreeNodeAddon
		{
			TreeNodeStatistic statistic;
			detail::BoardNodeMap board_node_map;
		};
	}
}