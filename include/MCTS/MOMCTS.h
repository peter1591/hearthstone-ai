#pragma once

#include "board/Board.h"
#include "MCTS/SOMCTS.h"

namespace mcts
{
	// multiple-observer MCTS
	class MOMCTS
	{
	public:
		MOMCTS(Statistic<> & statistic) :
			first_(state::kPlayerFirst, statistic),
			second_(state::kPlayerSecond, statistic)
		{}

		template <typename StartBoardGetter>
		void Iterate(StartBoardGetter&& start_board_getter)
		{
			state::State state = start_board_getter();

			first_.StartEpisode();
			second_.StartEpisode();

			while (true)
			{
				state::PlayerIdentifier side = state.GetCurrentPlayerId();
				
				board::Board board(state, side.GetSide());
				Result result = GetSOMCTS(side).PerformOwnTurnActions(board);
				
				assert(result != Result::kResultInvalid);
				if (result != Result::kResultNotDetermined) {
					first_.EpisodeFinished(result);
					second_.EpisodeFinished(result);
					break;
				}

				assert(state.GetCurrentPlayerId() == side.Opposite());

				GetSOMCTS(side.Opposite()).ApplyOthersActions(board);
			}
		}

	private:
		SOMCTS & GetSOMCTS(state::PlayerIdentifier side) {
			if (side.IsFirst()) return first_;
			else {
				assert(side.IsSecond());
				return second_;
			}
		}

	private:
		SOMCTS first_;
		SOMCTS second_;
	};
}