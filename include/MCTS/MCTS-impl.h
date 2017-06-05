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
		episode_state_.Start(start_board_getter(), tree_.GetRootNode());

		ActionParameterGetter action_parameter_getter(*this);
		RandomGenerator random_generator(*this);

		Result result = Result::kResultInvalid;
		while (true)
		{
			int choices = episode_state_.GetBoard().GetActionsCount();
			int choice = this->UserChooseAction(choices);
			result = episode_state_.GetBoard().ApplyAction(choice, random_generator, action_parameter_getter);

			if (result == Result::kResultNotDetermined) continue;

			break;
		}

		if (result == Result::kResultInvalid) {
			// TODO: the action is invalid
			// TODO: if an (sub-)action's all sub-actions are invalid, the (sub-)action should be invalidated
			return;
		}

		// TODO: do back propagation
	}

	inline int MCTS::UserChooseAction(int exclusive_max)
	{
		return ActionCallback(exclusive_max, false);
	}

	inline int MCTS::RandomChooseAction(int exclusive_max)
	{
		return ActionCallback(exclusive_max, true);
	}

	inline int MCTS::ActionCallback(int choices, bool random)
	{
		auto stage = episode_state_.GetStage();

		if (stage == detail::EpisodeState::kStageSelection) {
			int action = SelectAction(choices, random);
			TreeNode* next_node = episode_state_.GetTreeNode()->GetChild(action);
			episode_state_.StepNext(action, next_node);
			return action;
		}

		if (stage == detail::EpisodeState::kStageSimulation) {
			return Simulate(choices, random);
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
			episode_state_.SetToSimulationStage();
			return episode_state_.GetTreeNode()->ExpandAction();
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
		// TODO: use MCTS exploration formula to weight between exploitation and exploration
		return 0;
	}

	inline int MCTS::Simulate(int choices, bool random)
	{
		return std::rand() % choices; // TODO: use more stronger random generator?
	}
}
