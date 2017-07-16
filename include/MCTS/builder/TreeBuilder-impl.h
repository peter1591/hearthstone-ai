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
		// Note: can only be called when current player is the viewer of 'board'
		inline TreeBuilder::SelectResult TreeBuilder::PerformSelect(
			TreeNode * node, board::Board & board, 
			detail::BoardNodeMap & last_node_map, TreeUpdater * updater)
		{
			action_replayer_.Clear();

			assert(node);

			apply_state_.Start(board);
			assert(apply_state_.IsValid());

			// TODO [PROFILING]:
			// We save the board every time here. This is just for the case an invalid action is applied
			// Ideally, invalid actions should be rarely happened.
			// So if copy a board is comparatively expensive, we need to decide if this is benefitial.
			// Alternatively, we can just mark the choice as invalid, and restart the whole episode again.

			if (!node->GetAddon().last_apply_status.IsMarkSucceeded()) {
				// never apply succeeded --> save board to quickly re-try for invalid states
				apply_state_.SaveBoard();
			}

			assert(node->GetAddon().consistency_checker.CheckBoard(board.CreateView()));
			selection_stage_.StartNewMainAction(node);

			TreeBuilder::SelectResult perform_result(ApplyAction(
				node->GetAddon().action_analyzer, selection_stage_));
			if (perform_result.result == Result::kResultInvalid) {
				node->GetAddon().last_apply_status.MarkFailed();
				return Result::kResultInvalid;
			}
			node->GetAddon().last_apply_status.MarkSucceeded();

			assert(apply_state_.IsValid());

			// we use mutable here, since we will throw it away after all
			auto & traversed_path = selection_stage_.GetMutableTraversedPath();

			// construct a redirect node for the last action
			assert(perform_result.new_node_created == false);
			if (perform_result.result == Result::kResultNotDetermined) {
				perform_result.node = last_node_map.GetOrCreateNode(board, &perform_result.new_node_created);
			}
			else {
				assert(perform_result.node == nullptr);
			}
			traversed_path.back().ConstructRedirectNode(perform_result.node);

			if (!perform_result.new_node_created) {
				perform_result.new_node_created = selection_stage_.HasNewNodeCreated();
			}

			assert(updater);
			updater->PushBackNodes(traversed_path, perform_result.node);

			assert([&](builder::TreeBuilder::TreeNode* node) {
				if (perform_result.result != Result::kResultNotDetermined) return true;
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

			apply_state_.Start(board);
			assert(apply_state_.IsValid());

			// TODO [PROFILING]:
			// We save the board every time here. This is just for the case an invalid action is applied
			// Ideally, invalid actions should be rarely happened.
			// So if copy a board is comparatively expensive, we need to decide if this is benefitial.
			// Alternatively, we can just restart the episode, and hope this will not happen frequently
			apply_state_.SaveBoard();

			simulation_stage_.StartNewAction();

			board::BoardActionAnalyzer action_analyzer;

			Result result = ApplyAction(action_analyzer, simulation_stage_);

			assert(apply_state_.IsValid());

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
				int choices = apply_state_.GetBoard().GetActionsCount(action_analyzer);
				assert(choices > 0); // at least end-turn should be valid

				int choice = -1;
				if constexpr (is_simulation) {
					choice = this->ChooseSimulateAction(ActionType(ActionType::kMainAction), board::ActionChoices(choices));
				}
				else {
					choice = this->ChooseSelectAction(ActionType(ActionType::kMainAction), board::ActionChoices(choices));
				}

				if (apply_state_.IsValid()) {
					result = apply_state_.ApplyAction(choice, action_analyzer);
					if (result != Result::kResultInvalid) break;
				}

				statistic_.ApplyActionFailed(is_simulation);

				int rollbacks = stage_handler.ReportInvalidAction();

				if (!apply_state_.HasSavedBoard()) break;

				action_replayer_.RemoveLast(rollbacks);
				stage_handler.RestartAction();

				apply_state_.RestoreBoard();
				apply_state_.SetValid();
				action_replayer_.Restart();
			}

			statistic_.ApplyActionSucceeded(is_simulation);
			return result;
		}

		inline int TreeBuilder::ChooseSelectAction(ActionType action_type, board::ActionChoices const& choices)
		{
			int choice = -1;
			
			if (!action_replayer_.IsEnd()) {
				choice = action_replayer_.GetChoice(action_type);

				if (selection_stage_.ChooseAction(apply_state_.GetBoard(), action_type, choices, choice) < 0) {
					// invalid action during replay
					assert(false); // should not happen. we've already rollback #-of-invalid-actions as reported
					return -1;
				}

				action_replayer_.StepNext();
				return choice;
			}

			choice = selection_stage_.ChooseAction(apply_state_.GetBoard(), action_type, choices);

			if (choice >= 0) {
				action_replayer_.RecordChoice(action_type, choice);
				action_replayer_.StepNext();
			}
			else {
				apply_state_.SetInvalid();
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

			choice = simulation_stage_.ChooseAction(apply_state_.GetBoard(), action_type, choices);

			if (choice >= 0) {
				action_replayer_.RecordChoice(action_type, choice);
				action_replayer_.StepNext();
			}
			else {
				apply_state_.SetInvalid();
			}

			return choice;
		}
	}
}