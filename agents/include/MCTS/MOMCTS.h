#pragma once

#include "engine/view/Board.h"
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
			engine::Game game;
			game.SetStartState(start_board_getter());

			first_.StartEpisode();
			second_.StartEpisode();

			while (true)
			{
				state::PlayerIdentifier side = game.GetCurrentState().GetCurrentPlayerId();
				
				engine::Result result = GetSOMCTS(side).PerformOwnTurnActions(
					engine::view::Board(game, side.GetSide()));
				assert(result != engine::kResultInvalid);
				
				if (result != engine::kResultNotDetermined) {
					first_.EpisodeFinished(game.GetCurrentState(), result);
					second_.EpisodeFinished(game.GetCurrentState(), result);
					break;
				}

				assert(game.GetCurrentState().GetCurrentPlayerId() == side.Opposite());

				GetSOMCTS(side.Opposite()).ApplyOthersActions(
					engine::view::Board(game, side.Opposite().GetSide()));
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