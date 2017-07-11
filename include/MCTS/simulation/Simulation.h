#pragma once

#include "MCTS/Config.h"
#include "MCTS/simulation/ChoiceBlacklist.h"
#include "MCTS/policy/Simulation.h"

namespace mcts
{
	namespace simulation
	{
		class Simulation
		{
		public:
			void StartNewAction(ChoiceBlacklist & progress) {
				progress.Reset();
			}

			int ChooseAction(board::Board const& board,
				ActionType action_type,
				board::ActionChoicesGetter const& action_choices_getter,
				ChoiceBlacklist & progress)
			{
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

				progress.FillChoices(choices);

				assert([&]() {
					auto & addon = progress.GetCurrentNodeAddon();
					return addon.action_choice_checker.Check(action_type, action_choices);
				}());

				int choice = Simulate(board, action_type, choices, progress);
				if (choice < 0) return -1;

				progress.ApplyChoice(choice);
				return action_choices.Get(choice);
			}

			bool ApplyChoice(ActionType action_type, int choice,
				board::ActionChoicesGetter const& action_choices_getter,
				ChoiceBlacklist & progress) const
			{
				board::ActionChoices action_choices = action_choices_getter();
				assert(!action_choices.Empty());
				int choices = action_choices.Size();
				progress.FillChoices(choices);

				assert([&]() {
					auto & addon = progress.GetCurrentNodeAddon();
					return addon.action_choice_checker.Check(action_type, action_choices);
				}());

				if (!progress.IsValid(choice)) return false;

				progress.ApplyChoice(choice);
				return true;
			}

			void ReportInvalidAction(ChoiceBlacklist & progress) {
				progress.ReportInvalidChoice();
			}

			void RestartAction(ChoiceBlacklist & progress) {
				// Use a white-list-tree to record all viable (sub-)actions
				progress.Restart();
			}

		private:
			int Simulate(board::Board const& board, ActionType action_type, int choices,
				ChoiceBlacklist & progress) const
			{
				size_t valid_choices = progress.GetWhiteListCount();
				if (valid_choices <= 0) return -1;

				if (action_type.IsChosenRandomly()) {
					int rnd = StaticConfigs::SimulationPhaseRandomActionPolicy::GetRandom((int)valid_choices);
					return (int)progress.GetWhiteListItem((size_t)rnd);
				}

				int choice = StaticConfigs::SimulationPhaseSelectActionPolicy::GetChoice(
					policy::simulation::ChoiceGetter(progress), board
				);

				return choice;
			}
		};
	}
}