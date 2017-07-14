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
		// Never returns kResultInvalid. Will automatically retry if an invalid action is applied
		// Note: can only be called when current player is the viewer of 'board'
		inline TreeBuilder::SelectResult TreeBuilder::PerformSelect(
			TreeNode * node, board::Board & board, 
			detail::BoardNodeMap & last_node_map, TreeUpdater * updater)
		{
			action_replayer_.Clear();

			assert(node);

			episode_state_.Start(board);
			assert(episode_state_.IsValid());

			// TODO [PROFILING]:
			// We save the board every time here. This is just for the case an invalid action is applied
			// Ideally, invalid actions should be rarely happened.
			// So if copy a board is comparatively expensive, we need to decide if this is benefitial.
			// Alternatively, we can just mark the choice as invalid, and restart the whole episode again.
			episode_state_.GetBoard().SaveState();

			assert(node->GetAddon().consistency_checker.CheckBoard(board.CreateView()));
			selection_stage_.StartNewMainAction(node);

			TreeBuilder::SelectResult perform_result(ApplyAction(
				node->GetAddon().action_analyzer, selection_stage_));

			assert(episode_state_.IsValid());
			statistic_.ApplyActionSucceeded(false);

			perform_result.node = selection_stage_.FinishMainAction(
				last_node_map,
				episode_state_.GetBoard(),
				&perform_result.new_node_created);

			assert(updater);
			updater->PushBackNodes(selection_stage_.GetTraversedPath());

			assert([](builder::TreeBuilder::TreeNode* node) {
				if (!node->GetActionType().IsValid()) return true;
				return node->GetActionType().GetType() == ActionType::kMainAction;
			}(perform_result.node));

			return perform_result;
		}
		
		// Never returns kResultInvalid. Will automatically retry if an invalid action is applied
		// Note: can only be called when current player is the viewer of 'board'
		inline Result TreeBuilder::PerformSimulate(board::Board & board)
		{
			action_replayer_.Clear();

			episode_state_.Start(board);
			assert(episode_state_.IsValid());

			// TODO [PROFILING]:
			// We save the board every time here. This is just for the case an invalid action is applied
			// Ideally, invalid actions should be rarely happened.
			// So if copy a board is comparatively expensive, we need to decide if this is benefitial.
			// Alternatively, we can just restart the episode, and hope this will not happen frequently
			episode_state_.GetBoard().SaveState();

			simulation_stage_.StartNewAction();

			board::BoardActionAnalyzer action_analyzer;

			Result result = ApplyAction(action_analyzer, simulation_stage_);

			assert(episode_state_.IsValid());
			statistic_.ApplyActionSucceeded(true);

			return result;
		}

		template <typename StageHandler>
		inline Result TreeBuilder::ApplyAction(
			board::BoardActionAnalyzer & action_analyzer,
			StageHandler&& stage_handler)
		{
			assert(action_replayer_.Empty());

			constexpr bool is_simulation = std::is_same_v<
				std::decay_t<StageHandler>,
				simulation::Simulation>;

			// sometimes an action might in fact an INVALID action
			// here use a loop to retry on those cases
			Result result = Result::kResultInvalid;
			while (true) {
				int choices = episode_state_.GetBoard().GetActionsCount(action_analyzer);

				int choice = -1;
				if constexpr (is_simulation) {
					choice = this->ChooseSimulateAction(ActionType(ActionType::kMainAction), board::ActionChoices(choices));
				}
				else {
					choice = this->ChooseSelectAction(ActionType(ActionType::kMainAction), board::ActionChoices(choices));
				}

				if (episode_state_.IsValid()) {
					result = episode_state_.GetBoard().ApplyAction(
						choice, action_analyzer, random_generator_, action_parameter_getter_);
					if (result != Result::kResultInvalid) break;
				}

				statistic_.ApplyActionFailed(is_simulation);

				int rollbacks = stage_handler.ReportInvalidAction();
				action_replayer_.RemoveLast(rollbacks);
				stage_handler.RestartAction();

				episode_state_.GetBoard().RestoreState();
				episode_state_.SetValid();
				action_replayer_.Restart();
			}

			assert(result != Result::kResultInvalid);
			return result;
		}

		inline int TreeBuilder::ChooseSelectAction(ActionType action_type, board::ActionChoices const& choices)
		{
			int choice = -1;
			
			if (!action_replayer_.IsEnd()) {
				choice = action_replayer_.GetChoice(action_type);

				if (selection_stage_.ChooseAction(episode_state_.GetBoard(), action_type, choices, choice) < 0) {
					// invalid action during replay
					assert(false); // should not happen. we've already rollback #-of-invalid-actions as reported
					return -1;
				}

				action_replayer_.StepNext();
				return choice;
			}

			choice = selection_stage_.ChooseAction(episode_state_.GetBoard(), action_type, choices);

			if (choice >= 0) {
				action_replayer_.RecordChoice(action_type, choice);
				action_replayer_.StepNext();
			}
			else {
				episode_state_.SetInvalid();
			}

			return choice;
		}

		inline int TreeBuilder::ChooseSimulateAction(ActionType action_type, board::ActionChoices const& choices)
		{
			int choice = -1;

			if (!action_replayer_.IsEnd()) {
				choice = action_replayer_.GetChoice(action_type);

				if (!simulation_stage_.ApplyChoice(action_type, choice, choices)) {
					// invalid action during replay
					assert(false); // should not happen. we've already rollback #-of-invalid-actions as reported
					return -1;
				}

				action_replayer_.StepNext();
				return choice;
			}

			choice = simulation_stage_.ChooseAction(episode_state_.GetBoard(), action_type, choices);

			if (choice >= 0) {
				action_replayer_.RecordChoice(action_type, choice);
				action_replayer_.StepNext();
			}
			else {
				episode_state_.SetInvalid();
			}

			return choice;
		}
	}
}