#pragma once

#include <cstdlib>

#include "MCTS/builder/TreeBuilder.h"
#include "MCTS/board/BoardView-impl.h"
#include "MCTS/board/ActionParameterGetter-impl.h"
#include "MCTS/board/RandomGenerator-impl.h"
#include "MCTS/detail/BoardNodeMap-impl.h"

#include "MCTS/board/BoardActionAnalyzer.h"
#include "MCTS/board/BoardActionAnalyzer-impl.h"

namespace mcts
{
	namespace builder
	{
		inline TreeBuilder::PerformResult TreeBuilder::PerformOneAction(
			TreeNode * const node, Stage const stage, board::Board & board, TreeUpdater * const updater)
		{
			episode_state_.Start(stage, board);

			assert(episode_state_.GetStage() == stage);
			if (stage == kStageSelection) {
				assert(node->GetAddon().consistency_checker.CheckBoard(board.CreateView()));
				selection_stage_.StartMainAction(node);
			}

			// TODO [PROFILING]:
			// We save the board every time here. This is just for the case an invalid action is applied
			// Ideally, invalid actions should be rarely happened.
			// So if copy a board is comparatively expensive, we need to decide if this is benefitial.
			assert(episode_state_.IsValid());
			episode_state_.GetBoard().SaveState();

			if (episode_state_.GetStage() == kStageSelection) {
				selection_stage_.StartNewAction();
			}
			else {
				assert(episode_state_.GetStage() == kStageSimulation);
				simulation_stage_.StartNewAction();
			}

			// sometimes an action might in fact an INVALID action
			// here use a loop to retry on those cases
			Result result = Result::kResultInvalid;

			while (true)
			{
				int choices = episode_state_.GetBoard().GetActionsCount();
				int choice = this->ChooseAction(ActionType(ActionType::kMainAction), board::ActionChoices(choices));

				if (episode_state_.IsValid()) {
					result = episode_state_.GetBoard().ApplyAction(choice, random_generator_, action_parameter_getter_);
					if (result != Result::kResultInvalid) break;
				}

				if (episode_state_.GetStage() == kStageSelection) {
					selection_stage_.ReportInvalidAction();
					selection_stage_.RestartAction();
				}
				else {
					assert(episode_state_.GetStage() == kStageSimulation);
					simulation_stage_.ReportInvalidAction();
					simulation_stage_.RestartAction();
				}

				episode_state_.GetBoard().RestoreState();
				episode_state_.SetValid();

				statistic_.ApplyActionFailed();
			}

			// action applied successfully
			assert(episode_state_.IsValid());
			statistic_.ApplyActionSucceeded();

			TreeBuilder::PerformResult perform_result;
			perform_result.result = result;
			if (stage == kStageSelection) {
				selection_stage_.FinishMainAction(node, episode_state_.GetBoard(), &perform_result.new_node_created);

				assert(updater);
				perform_result.node = selection_stage_.GetCurrentNode();
				updater->PushBackNodes(selection_stage_.GetTraversedPath());

				assert([](builder::TreeBuilder::TreeNode* node) {
					if (!node->GetActionType().IsValid()) return true;
					return node->GetActionType().GetType() == ActionType::kMainAction;
				}(perform_result.node));
			}
			return perform_result;
		}

		inline int TreeBuilder::ChooseAction(ActionType action_type, board::ActionChoices const& choices)
		{
			return ActionCallback(action_type, choices);
		}

		inline int TreeBuilder::ActionCallback(ActionType action_type, board::ActionChoices const& choices)
		{
			assert(!choices.Empty());

			int choice = -1;
			if (episode_state_.GetStage() == kStageSelection) {
				choice = selection_stage_.GetAction(episode_state_.GetBoard(), action_type, choices);
			}
			else {
				assert(episode_state_.GetStage() == kStageSimulation);
				choice = simulation_stage_.GetAction(episode_state_.GetBoard(), action_type, choices);
			}

			if (choice < 0) {
				episode_state_.SetInvalid();
			}
			return choice;
		}
	}
}