#pragma once

#include <utility>
#include "MCTS/selection/Selection.h"
#include "MCTS/simulation/Simulation.h"
#include "MCTS/detail/EpisodeState.h"
#include "MCTS/Statistic.h"
#include "MCTS/ActionType.h"
#include "MCTS/MCTSUpdater.h"
#include "MCTS/board/BoardView.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"

namespace mcts
{
	// Traverse and build a game tree in a monte-carlo fashion
	// TODO: rename to TreeBuilder?
	class MCTS
	{
	public:
		typedef selection::TreeNode TreeNode;

		MCTS(Statistic<> & statistic) : action_parameter_getter_(*this), random_generator_(*this),
			statistic_(statistic), new_node_created_(false)
		{
		}

		struct PerformResult
		{
			bool new_node_created; // only valid if started in selection stage
			Result result;
			TreeNode * node;
		};
		PerformResult PerformOneAction(TreeNode * root, Stage stage, board::Board & board, MCTSUpdater & updater);

	public: // for callbacks: action-parameter-getter and random-generator
		int ChooseAction(ActionType action_type, int choices);

	private:
		int ActionCallback(ActionType action_type, int choices);

	private:
		board::ActionParameterGetter action_parameter_getter_;
		board::RandomGenerator random_generator_;

		detail::EpisodeState episode_state_;
		Statistic<> & statistic_;

		selection::Selection selection_stage_;
		simulation::Simulation simulation_stage_;

		bool new_node_created_;
	};
}