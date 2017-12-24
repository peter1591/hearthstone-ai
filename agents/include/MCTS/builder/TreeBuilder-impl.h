#pragma once

#include <cstdlib>

#include "engine/view/BoardView-impl.h"
#include "MCTS/builder/TreeBuilder.h"
#include "MCTS/detail/BoardNodeMap-impl.h"

namespace mcts
{
	namespace builder
	{
		// Note: can only be called when current player is the viewer of 'board'
		inline TreeBuilder::SelectResult TreeBuilder::PerformSelect(
			TreeNode * node, engine::view::Board const& board,
			detail::BoardNodeMap & last_node_map, TreeUpdater * updater)
		{
			assert(node);

			board_ = &board;

			assert(node->GetAddon().consistency_checker.CheckBoard(board.CreateView()));
			selection_stage_.StartNewMainAction(node);

			TreeBuilder::SelectResult perform_result(ApplyAction(selection_stage_));
			assert(perform_result.result != engine::kResultInvalid);

			// we use mutable here, since we will throw it away after all
			auto & traversed_path = selection_stage_.GetMutableTraversedPath();

			// mark the last action as a redirect node
			traversed_path.back().ConstructRedirectNode();

			bool new_node_created = false;
			if (perform_result.result == engine::kResultNotDetermined) {
				perform_result.node = last_node_map.GetOrCreateNode(board, &new_node_created);
				assert(perform_result.node);
			}
			else {
				assert(perform_result.node == nullptr);

				if (perform_result.result == engine::kResultFirstPlayerWin) {
					perform_result.node = mcts::selection::TreeNode::GetFirstPlayerWinNode();
				}
				else if (perform_result.result == engine::kResultDraw) {
					perform_result.node = mcts::selection::TreeNode::GetDrawNode();
				}
				else {
					assert(perform_result.result == engine::kResultSecondPlayerWin);
					perform_result.node = mcts::selection::TreeNode::GetSecondPlayerWinNode();
				}
				assert(perform_result.node != nullptr);
			}

			if (!new_node_created) {
				new_node_created = selection_stage_.HasNewNodeCreated();
			}

			assert(perform_result.change_to_simulation == false); // default value
			if (new_node_created) {
				perform_result.change_to_simulation = true;
			}
			else if (traversed_path.back().GetEdgeAddon()->GetChosenTimes() < StaticConfigs::kSwitchToSimulationUnderChosenTimes) {
				perform_result.change_to_simulation = true;
			}

			assert(updater);
			updater->PushBackNodes(traversed_path, perform_result.node);

			assert([&](builder::TreeBuilder::TreeNode* node) {
				if (perform_result.result != engine::kResultNotDetermined) return true;
				if (!node->GetActionType().IsValid()) return true;
				return node->GetActionType().GetType() == engine::ActionType::kMainAction;
			}(perform_result.node));

			return perform_result;
		}
		
		// Never returns kResultInvalid. Will automatically retry if an invalid action is applied
		// Note: can only be called when current player is the viewer of 'board'
		inline engine::Result TreeBuilder::PerformSimulate(engine::view::Board const& board)
		{
			board_ = &board;

			engine::Result result = simulation_stage_.CutoffCheck(*board_);
			assert(result != engine::kResultInvalid);
			if (result != engine::kResultNotDetermined) return result;

			return ApplyAction(simulation_stage_);
		}

		template <typename StageHandler>
		inline engine::Result TreeBuilder::ApplyAction(StageHandler&& stage_handler)
		{
			constexpr bool is_simulation = std::is_same_v<
				std::decay_t<StageHandler>,
				simulation::Simulation>;

			action_cb_.Initialize(board_->GetCurrentPlayerStateRefView().GetValidActionGetter());
			auto result = board_->ApplyAction(action_cb_);
			assert(result != engine::kResultInvalid);

			statistic_.ApplyActionSucceeded(is_simulation);
			return result;
		}

		inline int TreeBuilder::ChooseSelectAction(engine::ActionType action_type, engine::ActionChoices const& choices)
		{
			assert(!choices.Empty());
			int choice = selection_stage_.ChooseAction(*board_, action_type, choices);
			assert(choice >= 0); // always return a valid choice
			return choice;
		}

		inline int TreeBuilder::ChooseSimulateAction(engine::ActionType action_type, engine::ActionChoices const& choices)
		{
			int choice = simulation_stage_.ChooseAction(
				*board_,
				action_cb_.GetAnalyzer(),
				action_type, choices);
			assert(choice >= 0);
			return choice;
		}
	}
}