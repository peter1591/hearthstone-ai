#pragma once

#include "MCTS/SOMCTS.h"

namespace mcts
{
	// multiple-observer MCTS
	class MOMCTS
	{
	public:
		MOMCTS(selection::TreeNode & first_tree,
			selection::TreeNode & second_tree,
			Statistic<> & statistic,
			std::mt19937 & selection_rand, std::mt19937 & simulation_rand
		) :
			first_root_(&first_tree), first_(statistic, selection_rand, simulation_rand),
			second_root_(&second_tree), second_(statistic, selection_rand, simulation_rand)
		{}

		template <class... StartArgs>
		void Iterate(StartArgs&&... start_args)
		{
			side_controller_.StartEpisode(std::forward<StartArgs>(start_args)...);
			first_.StartEpisode(first_root_);
			second_.StartEpisode(second_root_);

			while (true)
			{
				StaticConfigs::SideController::Side side = side_controller_.GetActionSide();
				
				GetSOMCTS(side).StartActions();
				engine::Result result = engine::kResultInvalid;
				while (side_controller_.GetActionSide() == side) {
					result = GetSOMCTS(side).PerformAction(side_controller_.GetSideView(side));
					assert(result != engine::kResultInvalid);
					if (result != engine::kResultNotDetermined) break;
				}

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
			if (side.IsFirst()) return first_root_;
			else return second_root_;
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
		
		selection::TreeNode * first_root_;
		SOMCTS first_;
		
		selection::TreeNode * second_root_;
		SOMCTS second_;
	};
}