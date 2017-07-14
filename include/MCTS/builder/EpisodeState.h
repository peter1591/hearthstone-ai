#pragma once

#include <vector>
#include "MCTS/board/Board.h"
#include "MCTS/Types.h"

namespace mcts
{
	namespace builder
	{
		class EpisodeState
		{
		public:
			EpisodeState() : board_(nullptr), is_valid_(true) {}

			void Start(board::Board & board)
			{
				is_valid_ = true;
				board_ = &board;
			}

			board::Board const& GetBoard() const { return *board_; }
			board::Board & GetBoard() { return *board_; }

			void SetValid() { is_valid_ = true; }
			void SetInvalid() { is_valid_ = false; }
			bool IsValid() const { return is_valid_; }

		private:
			board::Board * board_;
			bool is_valid_;
		};
	}
}