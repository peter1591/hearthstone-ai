#pragma once

#include "MCTS/selection/Selection.h"
#include "MCTS/simulation/Simulation.h"
#include "MCTS/detail/EpisodeState.h"
#include "MCTS/detail/Statistic.h"
#include "MCTS/ActionType.h"
#include "MCTS/board/BoardView.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"

namespace mcts
{
	class MCTS
	{
	public:
		MCTS() : action_parameter_getter_(*this), random_generator_(*this) {}

		void StartEpisode();

		Result PerformOneAction(board::Board & board);

		void EpisodeFinished(Result result);

	public: // for callbacks: action-parameter-getter and random-generator
		int ChooseAction(ActionType action_type, int choices);

	public: // debug
		void PrintStatistic() { statistic_.PrintMessage(); }

	private:
		int ActionCallback(ActionType action_type, int choices);

		void SwitchToSimulationMode() {
			// We use a flag here, since we cannot switch to simulation mode
			// in sub-actions.
			flag_switch_to_simulation_ = true;
		}

	private:
		board::ActionParameterGetter action_parameter_getter_;
		board::RandomGenerator random_generator_;

		detail::EpisodeState episode_state_;
		detail::Statistic<> statistic_;

		selection::Selection selection_stage_;
		simulation::Simulation simulation_stage_;

		bool flag_switch_to_simulation_;
	};
}