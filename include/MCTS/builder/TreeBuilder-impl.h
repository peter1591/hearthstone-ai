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
			TreeNode * node, board::Board const& board, 
			detail::BoardNodeMap & last_node_map, TreeUpdater * updater)
		{
			assert(node);

			apply_state_.Start(board);
			assert(apply_state_.IsValid());

			assert(node->GetAddon().consistency_checker.CheckBoard(board.CreateView()));
			selection_stage_.StartNewMainAction(node);

			TreeBuilder::SelectResult perform_result(ApplyAction(
				flow_context_, node->GetAddon().action_analyzer, selection_stage_));
			if (perform_result.result == Result::kResultInvalid) {
				return Result::kResultInvalid;
			}

			assert(apply_state_.IsValid());

			// we use mutable here, since we will throw it away after all
			auto & traversed_path = selection_stage_.GetMutableTraversedPath();

			// mark the last action as a redirect node
			traversed_path.back().ConstructRedirectNode();

			assert(perform_result.new_node_created == false);
			if (perform_result.result == Result::kResultNotDetermined) {
				perform_result.node = last_node_map.GetOrCreateNode(board, &perform_result.new_node_created);
			}
			else {
				assert(perform_result.node == nullptr);

				if (perform_result.result == Result::kResultFirstPlayerWin) {
					perform_result.node = mcts::selection::TreeNode::GetFirstWinNode();
				}
				else {
					assert(perform_result.result == Result::kResultSecondPlayerWin);
					perform_result.node = mcts::selection::TreeNode::GetSecondWinNode();
				}
				assert(perform_result.node != nullptr);
			}

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
		inline Result TreeBuilder::PerformSimulate(board::Board const& board)
		{
			apply_state_.Start(board);
			assert(apply_state_.IsValid());

			simulation_stage_.GetActionAnalyzer().Reset();

			return ApplyAction(
				flow_context_,
				simulation_stage_.GetActionAnalyzer(), simulation_stage_);
		}

		template <typename StageHandler>
		inline Result TreeBuilder::ApplyAction(
			FlowControl::FlowContext & flow_context,
			board::BoardActionAnalyzer & action_analyzer,
			StageHandler&& stage_handler)
		{
			constexpr bool is_simulation = std::is_same_v<
				std::decay_t<StageHandler>,
				simulation::Simulation>;

			int choices = apply_state_.GetBoard().GetActionsCount(action_analyzer);
			assert(choices > 0); // at least end-turn should be valid

			int choice = -1;
			Result result = Result::kResultInvalid;
			if constexpr (is_simulation) {
				result = this->SimulationCutoffCheck();
				assert(result != Result::kResultInvalid);
				if (result != Result::kResultNotDetermined) return result;

				choice = this->ChooseSimulateAction(
					ActionType(ActionType::kMainAction),
					board::ActionChoices(choices));
			}
			else {
				choice = this->ChooseSelectAction(
					ActionType(ActionType::kMainAction),
					board::ActionChoices(choices));
			}

			if (!apply_state_.IsValid()) {
				stage_handler.ReportInvalidAction();
				statistic_.ApplyActionFailed(is_simulation);
				return Result::kResultInvalid;
			}

			result = apply_state_.ApplyAction(choice, flow_context, action_analyzer);
			if (result == Result::kResultInvalid) {
				stage_handler.ReportInvalidAction();
				statistic_.ApplyActionFailed(is_simulation);
				return Result::kResultInvalid;
			}

			statistic_.ApplyActionSucceeded(is_simulation);
			return result;
		}

		inline int TreeBuilder::ChooseSelectAction(ActionType action_type, board::ActionChoices const& choices)
		{
			int choice = selection_stage_.ChooseAction(apply_state_.GetBoard(), action_type, choices);

			if (choice < 0) {
				apply_state_.SetInvalid();
			}

			return choice;
		}

		inline Result TreeBuilder::SimulationCutoffCheck()
		{
			return simulation_stage_.CutoffCheck(apply_state_.GetBoard());
		}

		inline int TreeBuilder::ChooseSimulateAction(ActionType action_type, board::ActionChoices const& choices)
		{
			int choice = simulation_stage_.ChooseAction(
				apply_state_.GetBoard(),
				action_type, choices);

			if (choice < 0) {
				apply_state_.SetInvalid();
			}

			return choice;
		}
	}
}