#pragma once

#include "MCTS/detail/BoardNodeMap.h"
#include "MCTS/board/BoardView.h"

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

		class BoardViewChecker
		{
		public:
			bool Check(board::BoardView const& new_view) {
				if (!board_view) {
					board_view.reset(new board::BoardView(new_view));
					return true;
				}
				return *board_view == new_view;
			}

		private:
			std::unique_ptr<board::BoardView> board_view;
		};

		// Add abilities to tree node to use in SO-MCTS
		// Note: this will lived in *every* tree node, so careful about memory footprints
		struct TreeNodeAddon
		{
			TreeNodeStatistic statistic;
			BoardViewChecker board_view_checker; // TODO: debug only
			detail::BoardNodeMap board_node_map;
		};
	}
}