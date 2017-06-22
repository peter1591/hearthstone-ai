#pragma once

#include <vector>
#include "MCTS/board/Board.h"

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
			EpisodeState() : stage_(kStageSelection), is_valid_(true), board_(nullptr) {}

			void Start()
			{
				stage_ = kStageSelection;
				is_valid_ = true;
			}

			void StartAction(board::Board & board)
			{
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