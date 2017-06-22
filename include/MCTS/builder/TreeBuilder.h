#pragma once

#include <utility>
#include "MCTS/selection/Selection.h"
#include "MCTS/simulation/Simulation.h"
#include "MCTS/builder/EpisodeState.h"
#include "MCTS/Statistic.h"
#include "MCTS/ActionType.h"
#include "MCTS/MCTSUpdater.h"
#include "MCTS/board/BoardView.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"

namespace mcts
{
	namespace builder
	{
		// Traverse and build a game tree in a monte-carlo fashion
		class TreeBuilder
		{
		public:
			typedef selection::TreeNode TreeNode;

			TreeBuilder(Statistic<> & statistic) : action_parameter_getter_(*this), random_generator_(*this),
				statistic_(statistic), new_node_created_(false)
			{
			}

			struct PerformResult
			{
				Result result;
				bool new_node_created; // only valid if started in selection stage
				TreeNode * node; // only valid if started in selection stage
			};

			// TODO: the select action can ONLY be performed without hidden information
			// Maybe pass a BoardView, which can
			//   1. Extract board view for a specific side
			//   2. Apply action to underlying board
			//   3. Forbid to see the underlying board (with hidden information)
			PerformResult PerformSelect(TreeNode * node, board::Board & board, MCTSUpdater * updater) {
				return PerformOneAction(node, kStageSelection, board, updater);
			}

			PerformResult PerformSimulate(board::Board & board) {
				return PerformOneAction(nullptr, kStageSimulation, board, nullptr);
			}

		private:
			PerformResult PerformOneAction(
				TreeNode * const node, Stage const stage, board::Board & board, MCTSUpdater * const updater);

		public: // for callbacks: action-parameter-getter and random-generator
			int ChooseAction(ActionType action_type, int choices);

		private:
			int ActionCallback(ActionType action_type, int choices);

		private:
			board::ActionParameterGetter action_parameter_getter_;
			board::RandomGenerator random_generator_;

			EpisodeState episode_state_;
			Statistic<> & statistic_;

			selection::Selection selection_stage_;
			simulation::Simulation simulation_stage_;

			bool new_node_created_;
		};
	}
}