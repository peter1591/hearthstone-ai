#pragma once

#include <utility>
#include "MCTS/selection/Selection.h"
#include "MCTS/simulation/Simulation.h"
#include "MCTS/builder/EpisodeState.h"
#include "MCTS/Statistic.h"
#include "MCTS/ActionType.h"
#include "MCTS/MCTSUpdater.h"
#include "MCTS/board/BoardOnlineViewer.h"
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

			PerformResult PerformSelect(TreeNode * node, board::BoardOnlineViewer & board, MCTSUpdater * updater) {
				return PerformOneAction(node, kStageSelection, board, updater);
			}

			PerformResult PerformSimulate(board::BoardOnlineViewer & board) {
				return PerformOneAction(nullptr, kStageSimulation, board, nullptr);
			}

		private:
			PerformResult PerformOneAction(
				TreeNode * const node, Stage const stage, board::BoardOnlineViewer & board, MCTSUpdater * const updater);

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