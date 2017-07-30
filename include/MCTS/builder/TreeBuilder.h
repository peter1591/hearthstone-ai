#pragma once

#include <utility>
#include "MCTS/selection/Selection.h"
#include "MCTS/simulation/Simulation.h"
#include "MCTS/builder/ActionApplyState.h"
#include "MCTS/Statistic.h"
#include "MCTS/Types.h"
#include "MCTS/builder/TreeUpdater.h"
#include "MCTS/board/Board.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"
#include "MCTS/board/ActionChoices.h"
#include "MCTS/builder/ActionReplayer.h"

namespace mcts
{
	namespace builder
	{
		// Traverse and build a game tree in a monte-carlo fashion
		class TreeBuilder
		{
		public:
			typedef selection::TreeNode TreeNode;

			TreeBuilder(SOMCTS & caller, Statistic<> & statistic, int rand_seed) :
				statistic_(statistic),
				apply_state_(caller),
				selection_stage_(rand_seed), simulation_stage_(rand_seed)
			{
			}

			TreeBuilder(TreeBuilder const&) = delete;
			TreeBuilder & operator=(TreeBuilder const&) = delete;

			struct SelectResult
			{
				Result result; // Never returns kResultInvalid (automatically retry)
				bool new_node_created; // only valid if started in selection stage
				TreeNode * node; // only valid if started in selection stage

				SelectResult(Result new_result) :
					result(new_result),
					new_node_created(false),
					node(nullptr)
				{}
			};
			
			// Note: can only be called when current player is the viewer of 'board'
			SelectResult PerformSelect(TreeNode * node, board::Board const& board,
				detail::BoardNodeMap & last_node_map, TreeUpdater * updater);

			// Note: can only be called when current player is the viewer of 'board'
			Result PerformSimulate(board::Board const& board);

		private:
			template <typename StageHandler>
			Result ApplyAction(
				board::BoardActionAnalyzer & action_analyzer,
				StageHandler&& stage_handler);

		public: // for callbacks: action-parameter-getter and random-generator
			int ChooseSelectAction(ActionType action_type, board::ActionChoices const& choices);
			int ChooseSimulateAction(ActionType action_type, board::ActionChoices const& choices);

		private:
			Statistic<> & statistic_;

			ActionApplyState apply_state_;

			selection::Selection selection_stage_;
			simulation::Simulation simulation_stage_;
		};
	}
}