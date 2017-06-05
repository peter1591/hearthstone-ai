#pragma once

#include "MCTS/Tree.h"

#include "MCTS/detail/EpisodeState.h"

namespace mcts
{
	class MCTS
	{
	public:
		template <typename StartBoardGetter>
		void Iterate(StartBoardGetter&& start_board_getter);

		int UserChooseAction(int exclusive_max);
		int RandomChooseAction(int exclusive_max);

	private:
		int ActionCallback(int choices, bool random);

		int SelectAction(int choices, bool random);
		int SelectActionByRandom(int choices);
		int SelectActionByChoice(int choices);

		int Simulate(int choices, bool random);

	private:
		Tree tree_;
		detail::EpisodeState episode_state_;
	};
}