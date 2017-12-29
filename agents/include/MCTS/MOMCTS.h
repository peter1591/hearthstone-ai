#pragma once

#include "MCTS/SOMCTS.h"
#include "MCTS/builder/TreeBuilder-impl.h"

namespace mcts
{
	// multiple-observer MCTS
	class MOMCTS
	{
	public:
		MOMCTS(builder::TreeBuilder::TreeNode & first_tree,
			builder::TreeBuilder::TreeNode & second_tree,
			Statistic<> & statistic,
			std::mt19937 & selection_rand, std::mt19937 & simulation_rand
		) :
			first_(first_tree, statistic, selection_rand, simulation_rand),
			second_(second_tree, statistic, selection_rand, simulation_rand)
		{}

		template <class... StartArgs>
		void Iterate(StartArgs&&... start_args)
		{
			side_controller_.StartEpisode(std::forward<StartArgs>(start_args)...);
			first_.StartEpisode();
			second_.StartEpisode();

			while (true)
			{
				StaticConfigs::SideController::Side side = side_controller_.GetActionSide();
				
				engine::Result result = GetSOMCTS(side).PerformOwnTurnActions(
					side_controller_.GetSideView(side));
				assert(result != engine::kResultInvalid);
				
				if (result != engine::kResultNotDetermined) {
					first_.EpisodeFinished(
						side_controller_.GetSideView(StaticConfigs::SideController::Side::First()),
						result);
					second_.EpisodeFinished(
						side_controller_.GetSideView(StaticConfigs::SideController::Side::Second()),
						result);
					break;
				}

				GetSOMCTS(side.Opposite()).ApplyOthersActions(
					side_controller_.GetSideView(side.Opposite()));
			}
		}

		auto GetRootNode(StaticConfigs::SideController::Side side) const {
			return GetSOMCTS(side).GetRootNode();
		}

	private:
		SOMCTS & GetSOMCTS(StaticConfigs::SideController::Side side) {
			if (side.IsFirst()) return first_;
			else {
				assert(side.IsSecond());
				return second_;
			}
		}

		SOMCTS const& GetSOMCTS(StaticConfigs::SideController::Side side) const {
			if (side.IsFirst()) return first_;
			else {
				assert(side.IsSecond());
				return second_;
			}
		}

	private:
		StaticConfigs::SideController side_controller_;
		SOMCTS first_;
		SOMCTS second_;
	};
}