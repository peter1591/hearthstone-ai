#pragma once

#include "MCTS/board/BoardActionAnalyzer.h"
#include "MCTS/board/BoardActionAnalyzer-impl.h"
#include "MCTS/board/BoardView.h"

namespace mcts
{
	namespace board
	{
		class Board;
		class SavedBoard {
			friend class Board;

		public:
			SavedBoard() : state_() {}
			SavedBoard(SavedBoard const&) = delete;
			SavedBoard & operator=(SavedBoard const&) = delete;

		private:
			SavedBoard(state::State const& state) : state_(state) {}

			state::State state_;
		};

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
				return action_analyzer.GetActionsCount(
					FlowControl::CurrentPlayerStateView(board_));
			}

			Result ApplyAction(int action,
				BoardActionAnalyzer & action_analyzer,
				RandomGenerator & random, ActionParameterGetter & action_parameters)
			{
				assert(board_.GetCurrentPlayerId().GetSide() == side_);
				return action_analyzer.ApplyAction(
					FlowControl::CurrentPlayerStateView(board_),
					action, random, action_parameters);
			}

		public:
			void Save(SavedBoard & save) const {
				save.state_ = board_;
			}

			void Restore(SavedBoard const& save) {
				board_ = save.state_;
			}

		private:
			state::State & board_;
			state::PlayerSide side_;
		};
	}
}