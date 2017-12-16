#pragma once

#include "judge/IActionParameterGetter.h"
#include "MCTS/board/BoardActionAnalyzer.h"
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

			auto GetCurrentPlayerStateView() const {
				if (board_.GetCurrentPlayerId().GetSide() != side_) {
					assert(false);
					throw std::runtime_error("current player does not match.");
				}
				return FlowControl::CurrentPlayerStateView(board_);
			}

		public: // bridge to action analyzer
			Result ApplyAction(
				BoardActionAnalyzer & action_analyzer,
				state::IRandomGenerator & random, judge::IActionParameterGetter & action_parameters) const
			{
				assert(board_.GetCurrentPlayerId().GetSide() == side_);
				FlowControl::FlowContext flow_context(random, action_parameters);
				FlowControl::FlowController flow_controller(board_, flow_context);
				auto flow_result = flow_controller.PerformOperation();
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