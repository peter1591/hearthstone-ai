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

			int choices = episode_state_.GetBoard().GetActionsCount();
			int choice = this->UserChooseAction(choices);

			result = episode_state_.GetBoard().ApplyAction(choice, random_generator, action_parameter_getter);

			if (result == Result::kResultInvalid) {
				if (episode_state_.GetStage() == detail::EpisodeState::kStageSelection) {
					assert(episode_state_.GetTreeNode() == last_node_.GetChild());
					last_node_.RemoveNode();
				}
				return;
			}

			if (result != Result::kResultNotDetermined) break;
		}

		// TODO: do back propagation
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
		// TODO: use value network to enhance simulation
		return std::rand() % choices;
	}
}
