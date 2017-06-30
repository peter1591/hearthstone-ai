#pragma once

#include <utility>
#include "MCTS/selection/Selection.h"
#include "MCTS/simulation/Simulation.h"
#include "MCTS/builder/EpisodeState.h"
#include "MCTS/Statistic.h"
#include "MCTS/Types.h"
#include "MCTS/builder/TreeUpdater.h"
#include "MCTS/board/Board.h"
#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/board/RandomGenerator.h"
#include "MCTS/board/ActionChoices.h"

namespace mcts
{
	namespace builder
	{
		// Traverse and build a game tree in a monte-carlo fashion
		// To support share-node and eliminate random-nodes, we actually build
		// a flatten structure using hash table
		class TreeBuilder
		{
		public:
			typedef selection::TreeNode TreeNode;

			TreeBuilder(Statistic<> & statistic) : action_parameter_getter_(*this), random_generator_(*this),
				statistic_(statistic)
			{
			}

			struct PerformResult
			{
				Result result;
				bool new_node_created; // only valid if started in selection stage
				TreeNode * node; // only valid if started in selection stage

				PerformResult(Result new_result) :
					result(new_result),
					new_node_created(false),
					node(nullptr)
				{}
			};

			// Never returns kResultInvalid. Will automatically retry if an invalid action is applied
			// Note: can only be called when current player is the viewer of 'board'
			PerformResult PerformSelect(TreeNode * node, board::Board & board, TreeUpdater * updater) {
				return PerformOneAction(node, kStageSelection, board, updater);
			}

			// Never returns kResultInvalid. Will automatically retry if an invalid action is applied
			// Note: can only be called when current player is the viewer of 'board'
			PerformResult PerformSimulate(board::Board & board) {
				return PerformOneAction(nullptr, kStageSimulation, board, nullptr);
			}

		private:
			// Never returns kResultInvalid. Will automatically retry if an invalid action is applied
			// Note: 'node' is used only in selection stage (i.e., stage = kStageSelection)
			// Note: can only be called when current player is the viewer of 'board'
			PerformResult PerformOneAction(
				TreeNode * const node, Stage const stage, board::Board & board, TreeUpdater * const updater);

			Result ApplyAction() {
				if (episode_state_.GetStage() == kStageSelection) {
					return ApplyAction(selection_stage_);
				}
				else {
					assert(episode_state_.GetStage() == kStageSimulation);
					return ApplyAction(simulation_stage_);
				}
			}

			template <typename StageHandler>
			Result ApplyAction(StageHandler&& stage_handler);

		public: // for callbacks: action-parameter-getter and random-generator
			int ChooseAction(ActionType action_type, board::ActionChoices const& choices);

		private:
			board::ActionParameterGetter action_parameter_getter_;
			board::RandomGenerator random_generator_;

			EpisodeState episode_state_;
			Statistic<> & statistic_;

			selection::Selection selection_stage_;
			simulation::Simulation simulation_stage_;
		};
	}
}