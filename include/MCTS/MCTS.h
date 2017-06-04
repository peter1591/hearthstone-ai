#pragma once

#include "MCTS/Board.h"

namespace mcts
{
	class MCTS
	{
	public:
		template <typename StartBoardGetter>
		void Iterate(StartBoardGetter&& start_board_getter) {
			StartEpisode(std::forward<StartBoardGetter>(start_board_getter));
			Select();
			Expand();
			Simulate();
			BackPropagate();
		}

	private:
		template <typename StartBoardGetter>
		void StartEpisode(StartBoardGetter start_board_getter) {
			board_ = start_board_getter.GetBoard();
		}

		void Select() {

		}

		void Expand() {

		}

		void Simluate() {

		}

		void BackPropagate();

	private:
		Board board_;
	};
}