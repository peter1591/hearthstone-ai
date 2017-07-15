#pragma once

#include "MCTS/detail/BoardNodeMap.h"
#include "MCTS/board/BoardView.h"
#include "MCTS/board/ActionChoices.h"

namespace mcts
{
	namespace selection
	{
		class TreeNodeLastApplyStatus
		{
		public:
			TreeNodeLastApplyStatus() : success_(true) {}

			void MarkSucceeded() { success_ = true; }
			void MarkFailed() { success_ = false; }

			bool IsMarkSucceeded() const { return success_; }

		private:
			bool success_;
		};

		class TreeNodeConsistencyCheckAddons
		{
		public:
			TreeNodeConsistencyCheckAddons() : board_view_(), action_type_() {}

			bool Check(
				board::Board const& board,
				ActionType action_type,
				board::ActionChoices const& choices)
			{
				assert(action_type.IsValid());
				if (!CheckActionType(action_type)) return false;
				if (!CheckBoard(board.CreateView())) return false;
				return true;
			}

			bool CheckBoard(board::BoardView const& new_view) {
				if (!board_view_) {
					board_view_.reset(new board::BoardView(new_view));
					return true;
				}
				return *board_view_ == new_view;
			}

		private:
			bool CheckActionType(ActionType action_type) {
				if (!action_type_.IsValid()) {
					action_type_ = action_type;
					return true;
				}
				return action_type_ == action_type;
			}

		private:
			std::unique_ptr<board::BoardView> board_view_;
			ActionType action_type_;
		};

		// Add abilities to tree node to use in SO-MCTS
		// Note: this will lived in *every* tree node, so careful about memory footprints
		struct TreeNodeAddon
		{
			TreeNodeAddon() :
				action_analyzer(),
				consistency_checker(),
				board_node_map(),
				last_apply_status()
			{}

			board::BoardActionAnalyzer action_analyzer;
			TreeNodeConsistencyCheckAddons consistency_checker; // TODO: debug only
			detail::BoardNodeMap board_node_map;
			TreeNodeLastApplyStatus last_apply_status;
		};
	}
}