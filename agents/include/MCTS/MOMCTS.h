#pragma once

#include "MCTS/SOMCTS.h"

namespace mcts
{
	// multiple-observer MCTS
	class MOMCTS
	{
	public:
		MOMCTS(
			selection::TreeNode & first_tree,
			selection::TreeNode & second_tree,
			Statistic<> & statistic,
			std::mt19937 & selection_rand, std::mt19937 & simulation_rand,
			Config const& config
		) :
			first_(first_tree, statistic, selection_rand, simulation_rand, config),
			second_(second_tree, statistic, selection_rand, simulation_rand, config)
		{}

		template <class... StartArgs>
		void Iterate(StartArgs&&... start_args)
		{
			side_controller_.StartEpisode(std::forward<StartArgs>(start_args)...);
			first_.StartIteration();
			second_.StartIteration();

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
					first_.FinishIteration(
						side_controller_.GetSideView(StaticConfigs::SideController::Side::First()),
						result);
					second_.FinishIteration(
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