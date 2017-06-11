#pragma once

#include <vector>
#include "MCTS/Board.h"

namespace mcts
{
	namespace detail
	{
		class EpisodeState
		{
		public:
			enum Stage {
				kStageSelection,
				kStageSimulation
			};

		public:
			EpisodeState() : stage_(kStageSelection), is_valid_(true) {}

			void Start(Board const& board)
			{
				stage_ = kStageSelection;
				board_ = board;
			}

			Stage GetStage() const { return stage_; }
			void SetToSimulationStage() { stage_ = kStageSimulation; }

			void SetBoard(Board const& board) { board_ = board; }
			Board const& GetBoard() const { return board_; }
			Board & GetBoard() { return board_; }

			void SetValid() { is_valid_ = true; }
			void SetInvalid() { is_valid_ = false; }
			bool IsValid() const { return is_valid_; }

		private:
			Board board_;
			Stage stage_;
			bool is_valid_;
		};
	}
}