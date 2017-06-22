#pragma once

#include <vector>
#include "MCTS/board/Board.h"
#include "MCTS/Stage.h"

namespace mcts
{
	namespace builder
	{
		class EpisodeState
		{
		public:
			EpisodeState() : stage_(kStageSelection), is_valid_(true), board_(nullptr) {}

			void Start(Stage stage, board::Board & board)
			{
				stage_ = stage;
				is_valid_ = true;
				board_ = &board;
			}

			Stage GetStage() const { return stage_; }
			void SetToSimulationStage() { stage_ = kStageSimulation; }

			board::Board const& GetBoard() const { return *board_; }
			board::Board & GetBoard() { return *board_; }

			void SetValid() { is_valid_ = true; }
			void SetInvalid() { is_valid_ = false; }
			bool IsValid() const { return is_valid_; }

		private:
			board::Board * board_;
			Stage stage_;
			bool is_valid_;
		};
	}
}