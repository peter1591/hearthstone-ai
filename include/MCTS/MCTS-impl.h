#pragma once

#include <cstdlib>

#include "MCTS/MCTS.h"
#include "MCTS/board/Board-impl.h"
#include "MCTS/board/BoardView-impl.h"
#include "MCTS/board/ActionParameterGetter-impl.h"
#include "MCTS/board/RandomGenerator-impl.h"

namespace mcts
{
	template <typename StartBoardGetter>
	inline void MCTS::StartEpisode(StartBoardGetter && start_board_getter) {
		flag_switch_to_simulation_ = false;
		episode_state_.Start(start_board_getter());
		selection_stage_.StartEpisode();
	}

	// Never returns kResultInvalid
	//    Will automatically retry if an invalid action is applied
	inline Result MCTS::PerformOneAction() {
		if (flag_switch_to_simulation_) {
			episode_state_.SetToSimulationStage();
			flag_switch_to_simulation_ = false;
		}

		// TODO [PROFILING]:
		// we save the board every time just in case an invalid action is applied
		// ideally, invalid actions should be rarely happened.
		// Need to decide if this is benefitial.
		assert(episode_state_.IsValid());
		board::Board const saved_board = episode_state_.GetBoard();

		if (episode_state_.GetStage() == detail::EpisodeState::kStageSelection) {
			selection_stage_.StartNewAction();
		}
		else {
			assert(episode_state_.GetStage() == detail::EpisodeState::kStageSimulation);
			simulation_stage_.StartNewAction();
		}

		// sometimes an action might in fact an INVALID action
		// here use a loop to retry on those cases
		while (true)
		{
			int choices = episode_state_.GetBoard().GetActionsCount();
			int choice = this->ChooseAction(ActionType(ActionType::kMainAction), choices);

			Result result = Result::kResultInvalid;
			if (episode_state_.IsValid()) {
				result = episode_state_.GetBoard().ApplyAction(choice, random_generator_, action_parameter_getter_);
			}

			if (result == Result::kResultInvalid) {
				if (episode_state_.GetStage() == detail::EpisodeState::kStageSelection) {
					selection_stage_.ReportInvalidAction();
					selection_stage_.RestartAction();
				}
				else {
					assert(episode_state_.GetStage() == detail::EpisodeState::kStageSimulation);
					simulation_stage_.ReportInvalidAction();
					simulation_stage_.RestartAction();
				}

				episode_state_.SetBoard(saved_board);
				episode_state_.SetValid();

				statistic_.ApplyActionFailed();
				continue;
			}

			statistic_.ApplyActionSucceeded();

			return result; // action applied successfully
		}
	}

	inline void MCTS::EpisodeFinished(Result result) {
		assert(result != Result::kResultInvalid);
		bool win = (result == Result::kResultFirstPlayerWin);
		selection_stage_.ReportResult(win);
	}

	inline int MCTS::ChooseAction(ActionType action_type, int choices)
	{
		return ActionCallback(action_type, choices);
	}

	inline int MCTS::ActionCallback(ActionType action_type, int choices)
	{
		assert(choices > 0);

		auto stage = episode_state_.GetStage();
		int choice = -1;

		if (stage == detail::EpisodeState::kStageSelection) {
			// if a new node is created, we switch to simulation
			bool & created_new_node = flag_switch_to_simulation_;
			choice = selection_stage_.GetAction(episode_state_.GetBoard(), action_type, choices, &created_new_node);
		}
		else {
			assert(stage == detail::EpisodeState::kStageSimulation);
			choice = simulation_stage_.GetAction(episode_state_.GetBoard(), action_type, choices);
		}

		if (choice < 0)
			episode_state_.SetInvalid();
		return choice;
	}
}
