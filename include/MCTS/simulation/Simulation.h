#pragma once

#include "MCTS/Config.h"
#include "MCTS/policy/Simulation.h"

namespace mcts
{
	namespace simulation
	{
		class Simulation
		{
		public:
			void StartNewAction() {
			}

			int ChooseAction(
				board::Board const& board,
				ActionType action_type,
				board::ActionChoices const& action_choices)
			{
				if (action_choices.Empty()) return -1;

				int choices = action_choices.Size();

				if (choices == 1) {
					return 0;
				}

				if (action_type.IsChosenRandomly()) {
					int rnd = StaticConfigs::SimulationPhaseRandomActionPolicy::GetRandom(choices);
					return action_choices.Get(rnd);
				}

				assert(action_type.IsChosenManually());

				// The underlying policy treat all choices as continuous numbers from zero
				int choice = StaticConfigs::SimulationPhaseSelectActionPolicy::GetChoice(
					policy::simulation::ChoiceGetter(choices), board
				);
				if (choice < 0) return -1;

				return action_choices.Get(choice);
			}

			void ReportInvalidAction() {
			}
		};
	}
}