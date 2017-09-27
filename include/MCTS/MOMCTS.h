#pragma once

#include "board/Board.h"
#include "MCTS/SOMCTS.h"
#include "MCTS/builder/TreeBuilder-impl.h"

namespace mcts
{
	// multiple-observer MCTS
	// The first player is defined to be the player AI is helping
	class MOMCTS
	{
	public:
		MOMCTS(builder::TreeBuilder::TreeNode & first_tree,
			builder::TreeBuilder::TreeNode & second_tree,
			Statistic<> & statistic,
			std::mt19937 & selection_rand, std::mt19937 & simulation_rand
		) :
			first_(state::kPlayerFirst, first_tree, statistic, selection_rand, simulation_rand),
			second_(state::kPlayerSecond, second_tree, statistic, selection_rand, simulation_rand)
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
				
				Result result = GetSOMCTS(side).PerformOwnTurnActions(
					board::Board(state, side.GetSide()));
				assert(result != Result::kResultInvalid);
				
				if (result != Result::kResultNotDetermined) {
					first_.EpisodeFinished(result);
					second_.EpisodeFinished(result);
					break;
				}

				assert(state.GetCurrentPlayerId() == side.Opposite());

				GetSOMCTS(side.Opposite()).ApplyOthersActions(
					board::Board(state, side.Opposite().GetSide()));
			}
		}

		auto GetRootNode(state::PlayerIdentifier side) const {
			return GetSOMCTS(side).GetRootNode();
		}

	private:
		SOMCTS & GetSOMCTS(state::PlayerIdentifier side) {
			if (side.IsFirst()) return first_;
			else {
				assert(side.IsSecond());
				return second_;
			}
		}

		SOMCTS const& GetSOMCTS(state::PlayerIdentifier side) const {
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