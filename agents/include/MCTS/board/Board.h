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

		class CopiedBoard;

		// Make sure no hidden information is accessed by a player
		class Board
		{
			friend CopiedBoard;

		public:
			Board(state::State & board, state::PlayerSide side) : board_(board), side_(side) {}

		public:
			state::PlayerIdentifier GetCurrentPlayer() const {
				return board_.GetCurrentPlayerId();
			}

			state::PlayerSide GetViewSide() const { return side_; }

			BoardView CreateView() const {
				if (side_ == state::kPlayerFirst) {
					return BoardView(FlowControl::PlayerStateView<state::kPlayerFirst>(board_));
				}
				else {
					assert(side_ == state::kPlayerSecond);
					return BoardView(FlowControl::PlayerStateView<state::kPlayerSecond>(board_));
				}
			}

			template <class Functor>
			auto ApplyWithPlayerStateView(Functor && functor) const {
				if (side_ == state::kPlayerFirst) {
					return functor(FlowControl::PlayerStateView<state::kPlayerFirst>(board_));
				}
				else {
					assert(side_ == state::kPlayerSecond);
					return functor(FlowControl::PlayerStateView<state::kPlayerSecond>(board_));
				}
			}

		public: // bridge to action analyzer
			void PrepareActionAnalyzer(BoardActionAnalyzer & action_analyzer) const
			{
				// board action analyzer will not access hidden information
				//   as long as the current-player is the viewer
				assert(board_.GetCurrentPlayerId().GetSide() == side_);
				return action_analyzer.Prepare(FlowControl::CurrentPlayerStateView(board_));
			}

			Result ApplyAction(
				BoardActionAnalyzer & action_analyzer,
				FlowControl::FlowContext & flow_context,
				IRandomGenerator & random, IActionParameterGetter & action_parameters) const
			{
				assert(board_.GetCurrentPlayerId().GetSide() == side_);
				auto flow_result = action_analyzer.GetActionApplierByRefThis().Apply(
					board_, action_parameters, random);
				return Result::ConvertFrom(flow_result);
			}

		public:
			void Save(SavedBoard & save) const {
				save.state_ = board_;
			}

			void Restore(SavedBoard const& save) const {
				board_ = save.state_;
			}

		public:
			state::State const& RevealHiddenInformationForSimulation() const {
				// For simulation, it needs state information to estimate who's going to win
				// Hidden information is sometimes a big boost to make a better predict.
				// NOTE: THIS SHOULD ONLY BE USED FOR SIMULATION STATE-VALUE ESTIMATION
				return board_;
			}

		private:
			state::State & board_;
			state::PlayerSide side_;
		};

		class CopiedBoard {
		public:
			CopiedBoard(state::PlayerSide side) : state_(), board_(state_, side) {}

			void FillWithBase(Board const& board) {
				state_.FillWithBase(board.board_);
			}

			Board & GetBoard() { return board_; }

		private:
			state::State state_;
			Board board_;
		};
	}
}