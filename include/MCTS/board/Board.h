#pragma once

#include "MCTS/board/BoardActionAnalyzer.h"
#include "MCTS/board/BoardActionAnalyzer-impl.h"
#include "MCTS/board/BoardView.h"

namespace mcts
{
	namespace board
	{
		// Make sure no hidden information is accessed by a player
		class Board
		{
		public:
			Board(state::State & board, state::PlayerSide side) : board_(board), side_(side) {}

		public:
			state::PlayerIdentifier GetCurrentPlayer() const {
				return board_.GetCurrentPlayerId();
			}

			BoardView CreateView() const {
				return BoardView(side_, board_);
			}

		public: // bridge to action analyzer
			int GetActionsCount(BoardActionAnalyzer & action_analyzer) const
			{
				// board action analyzer will not access hidden information
				//   as long as the current-player is the viewer
				assert(board_.GetCurrentPlayerId().GetSide() == side_);
				return action_analyzer.GetActionsCount(board_);
			}

			Result ApplyAction(int action,
				BoardActionAnalyzer & action_analyzer,
				RandomGenerator & random, ActionParameterGetter & action_parameters)
			{
				assert(board_.GetCurrentPlayerId().GetSide() == side_);
				return action_analyzer.ApplyAction(board_, action, random, action_parameters);
			}

		public:
			void SaveState() { saved_board_ = board_; }
			void RestoreState() { board_ = saved_board_; }

		private:
			state::State & board_;
			state::PlayerSide side_;
			state::State saved_board_;
		};
	}
}