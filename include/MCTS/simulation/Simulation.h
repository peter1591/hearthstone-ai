#pragma once

#include "MCTS/Config.h"
#include "MCTS/simulation/Tree.h"
#include "MCTS/policy/Simulation.h"

namespace mcts
{
	namespace simulation
	{
		class Simulation
		{
		public:
			Simulation() : root_(), progress_(&root_) {}

			void StartNewAction() {
				root_.Reset();
				progress_.Reset(&root_);
			}

			int ChooseAction(board::Board const& board, ActionType action_type, board::ActionChoicesGetter const& action_choices_getter) {
				board::ActionChoices action_choices = action_choices_getter();
				if (action_choices.Empty()) return -1;

				// The underlying tree treat all choices as continuous numbers from zero
				// However, the action_choices might be with two types:
				//   1. Choose a number from zero to a exclusive-max value
				//   2. Choose a card id from a few options
				// The first type is easy to handle. Since the underlying tree already treat all
				//    choices as numbers from zero
				// To handle the second type, we first noticed that,
				//    The choices of the card ids are THE SAME for a particular tree node
				//    The choices of the card ids might be chosen randomly, but since the random
				//    number is effectively FIXED to this node (a parent node represents the random
				//    outcome, and a different random number leads to a different child node)
				//    So, the choices of the card ids should be identical.
				// Thus, we can just use the zero-based index of the card id choices

				int choices = action_choices.Size();

				progress_.FillChoices(choices);

				assert([&]() {
					auto addon = progress_.GetCurrentNodeAddon();
					if (!addon) return true;
					return addon->action_choice_checker.Check(action_type, action_choices);
				}());

				int choice = Simulate(board, action_type, choices);
				if (choice < 0) return -1;

				progress_.ApplyChoice(choice);
				return action_choices.Get(choice);
			}

			void ReportInvalidAction() {
				progress_.ReportInvalidChoice();
			}

			void RestartAction() {
				// Use a white-list-tree to record all viable (sub-)actions
				progress_.Reset(&root_);
			}

		private:
			int Simulate(board::Board const& board, ActionType action_type, int choices) const
			{
				size_t valid_choices = progress_.GetWhiteListCount();
				if (valid_choices <= 0) return -1;

				if (action_type.IsChosenRandomly()) {
					int rnd = StaticConfigs::SimulationPhaseRandomActionPolicy::GetRandom((int)valid_choices);
					return (int)progress_.GetWhiteListItem((size_t)rnd);
				}

				int choice = StaticConfigs::SimulationPhaseSelectActionPolicy::GetChoice(
					policy::simulation::ChoiceGetter(progress_), board
				);

				return choice;
			}

		private:
			TreeNode root_;
			TreeTraverseProgress progress_;
		};
	}
}