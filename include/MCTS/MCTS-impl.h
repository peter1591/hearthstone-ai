#pragma once

#include <cstdlib>

#include "MCTS/MCTS.h"
#include "MCTS/Board-impl.h"

#include "MCTS/ActionParameterGetter.h"
#include "MCTS/ActionParameterGetter.h"

namespace mcts
{
	template <typename StartBoardGetter>
	inline void MCTS::Iterate(StartBoardGetter&& start_board_getter) {
		flag_switch_to_simulation_ = false;
		episode_state_.Start(start_board_getter(), tree_.GetRootNode());

		ActionParameterGetter action_parameter_getter(*this);
		RandomGenerator random_generator(*this);

		Result result = Result::kResultInvalid;

		while (true)
		{
			if (flag_switch_to_simulation_) {
				episode_state_.SetToSimulationStage();
				flag_switch_to_simulation_ = false;
			}

			detail::EpisodeState resume_state;

			if (episode_state_.GetStage() == detail::EpisodeState::kStageSimulation) {
				choice_white_list_.Clear();
				resume_state = episode_state_;
			}

			while (true)
			{
				int choices = episode_state_.GetBoard().GetActionsCount();
				int choice = this->UserChooseAction(choices);

				result = episode_state_.GetBoard().ApplyAction(choice, random_generator, action_parameter_getter);

				if (result == Result::kResultInvalid) {
					if (episode_state_.GetStage() == detail::EpisodeState::kStageSelection) {
						// We modify the tree structure, so next time this invalid node will not be chosen
						assert(episode_state_.GetTreeNode() == last_node_.GetChild());
						last_node_.RemoveNode();
						return;
					}

					assert(episode_state_.GetStage() == detail::EpisodeState::kStageSimulation);
					// Use a white-list-tree to record all viable (sub-)actions
					episode_state_ = resume_state;
					choice_white_list_.ReportInvalidChoice();
					choice_white_list_.Restart();
					continue;
				}
				break;
			}

			if (result != Result::kResultNotDetermined) break;
		}

		assert(result != Result::kResultInvalid);
		bool win = (result == Result::kResultFirstPlayerWin);

		std::for_each(
			episode_state_.GetTraversedPath().begin(),
			episode_state_.GetTraversedPath().end(),
			[&](TreeNode* traversed_node)
		{
			traversed_node->ReportResult(win);
		});
	}

	// TODO: add action type for simulation
	inline int MCTS::UserChooseAction(int exclusive_max)
	{
		return ActionCallback(exclusive_max, false);
	}

	// TODO: unify with UserChooseAction(), distinguishing by action type
	inline int MCTS::RandomChooseAction(int exclusive_max)
	{
		return ActionCallback(exclusive_max, true);
	}

	inline int MCTS::ActionCallback(int choices, bool random)
	{
		auto stage = episode_state_.GetStage();

		if (stage == detail::EpisodeState::kStageSelection) {
			int choice = SelectAction(choices, random);

			TreeNode* parent_node = episode_state_.GetTreeNode();
			TreeNode* next_node = parent_node->GetOrCreateChild(choice);
			episode_state_.StepNext(choice, next_node);

			last_node_.Set(parent_node, next_node, choice);

			return choice;
		}

		if (stage == detail::EpisodeState::kStageSimulation) {
			choice_white_list_.FillChoices(choices);
			int choice = Simulate(choices, random);
			choice_white_list_.ApplyChoice(choice);
			return choice;
		}

		assert(false);
		return 0;
	}

	inline int MCTS::SelectAction(int choices, bool random) {
		if (episode_state_.GetTreeNode()->NeedFillActions()) {
			episode_state_.GetTreeNode()->FillActions(choices, random);
		}

		assert(episode_state_.GetTreeNode()->GetActionCount() == choices);
		assert(episode_state_.GetTreeNode()->GetActionIsRandom() == random);

		// Check if current tree node has un-expanded action
		//   If yes, choose that action
		if (episode_state_.GetTreeNode()->HasUnExpandedAction()) {
			SwitchToSimulationMode();
			return episode_state_.GetTreeNode()->ExpandAction();
		}

		if (episode_state_.GetTreeNode()->GetChildren().empty()) {
			// no valid action from this node
			// so this node must be an invalid action
			// --> remove it from parent
			assert(last_node_.GetChild() == episode_state_.GetTreeNode());
			last_node_.RemoveNode();
		}
		
		if (random) return SelectActionByRandom(choices);
		else return SelectActionByChoice(choices);
	}

	inline int MCTS::SelectActionByRandom(int choices)
	{
		return std::rand() % choices; // TODO: use more stronger random generator?
	}

	inline int MCTS::SelectActionByChoice(int choices)
	{
		// TODO: balance between exploitation and exploration
		return 0;
	}

	inline int MCTS::Simulate(int choices, bool random)
	{
		assert(choice_white_list_.GetWhiteListCount() > 0);
		int final_choice = 0;
		choice_white_list_.ForEachWhiteListItem([&](int choice) {
			final_choice = choice;
			return false; // early stop
		});
		return final_choice;
		// TODO: use value network to enhance simulation
		//return std::rand() % choices;
	}
}
