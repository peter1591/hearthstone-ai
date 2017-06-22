#pragma once

#include "MCTS/board/Board.h"
#include "MCTS/board/BoardActionAnalyzer.h"
#include "MCTS/board/BoardActionAnalyzer-impl.h"

namespace mcts
{
	namespace board
	{
		// Make sure no hidden information is accessed by a player
		class BoardOnlineViewer
		{
		public:
			BoardOnlineViewer(Board & board, state::PlayerSide side) : board_(board), side_(side) {}

		public: // bridge to action analyzer
			int GetActionsCount()
			{
				// board action analyzer will not access hidden information
				//   as long as the current-player is the viewer
				assert(board_.GetCurrentPlayerId().GetSide() == side_);
				return action_analyzer_.GetActionsCount(board_);
			}

			Result ApplyAction(int action, RandomGenerator & random, ActionParameterGetter & action_parameters)
			{
				assert(board_.GetCurrentPlayerId().GetSide() == side_);
				return action_analyzer_.ApplyAction(board_, action, random, action_parameters);
			}

		public:
			void SaveState() { saved_board_ = board_; }
			void RestoreState() { board_ = saved_board_; }

		private:
			Board & board_;
			state::PlayerSide side_;

			BoardActionAnalyzer action_analyzer_;

			Board saved_board_;
		};
	}
}