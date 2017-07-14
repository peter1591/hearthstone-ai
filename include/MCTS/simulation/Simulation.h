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
			Simulation() : blacklist_() {}

			void StartNewAction() {
				blacklist_.Reset();
			}

			int ChooseAction(board::Board const& board,
				ActionType action_type,
				board::ActionChoices const& action_choices)
			{
				if (action_choices.Empty()) return -1;

				int choices = action_choices.Size();

				if (action_type.IsChosenRandomly()) {
					// ChoiceBlacklist do not care about random actions
					int rnd = StaticConfigs::SimulationPhaseRandomActionPolicy::GetRandom(choices);
					return action_choices.Get(rnd);
				}

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

				blacklist_.FillChoices(choices);

				assert([&]() {
					auto * addon = blacklist_.GetCurrentNodeAddon();
					if (!addon) return true;
					return addon->action_choice_checker.Check(action_type, action_choices);
				}());

				int choice = Simulate(board, action_type, choices);
				if (choice < 0) return -1;

				blacklist_.ApplyChoice(choice);
				return action_choices.Get(choice);
			}

			bool ApplyChoice(ActionType action_type, int choice,
				board::ActionChoices const& action_choices)
			{
				if (action_type.IsChosenRandomly()) {
					// ChoiceBlacklist do not care about random actions
					return true;
				}

				assert(!action_choices.Empty());
				int choices = action_choices.Size();
				blacklist_.FillChoices(choices);

				assert([&]() {
					auto * addon = blacklist_.GetCurrentNodeAddon();
					if (!addon) return true;
					return addon->action_choice_checker.Check(action_type, action_choices);
				}());

				if (!blacklist_.IsValid(choice)) return false;

				blacklist_.ApplyChoice(choice);
				return true;
			}

			// @return #-of-invalid-steps in this trial
			int ReportInvalidAction() {
				return blacklist_.ReportInvalidChoice();
			}

			void RestartAction() {
				blacklist_.Restart();
			}

		private:
			int Simulate(board::Board const& board, ActionType action_type, int choices) const
			{
				size_t valid_choices = blacklist_.GetWhiteListCount(choices);
				if (valid_choices <= 0) return -1;

				assert(action_type.IsChosenManually());

				int choice = StaticConfigs::SimulationPhaseSelectActionPolicy::GetChoice(
					policy::simulation::ChoiceGetter(choices, blacklist_), board
				);

				return choice;
			}

		private:
			ChoiceBlacklist blacklist_;
		};
	}
}