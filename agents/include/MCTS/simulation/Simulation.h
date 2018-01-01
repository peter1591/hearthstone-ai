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
			Simulation(std::mt19937 & rand) :
				random_(rand), select_(rand)
			{}

			engine::Result CutoffCheck(engine::view::Board const& board)
			{
				using Policy = std::decay_t<decltype(select_)>;

				if constexpr (Policy::kEnableCutoff) {
					return select_.GetCutoffResult(board);
				}
				else {
					return engine::kResultNotDetermined;
				}
			}
			
			void StartAction(engine::view::Board const& board, engine::ValidActionAnalyzer const& action_analyzer) {
				select_.StartAction(board, action_analyzer);
			}

			int ChooseAction(
				engine::view::Board const& board,
				engine::ValidActionAnalyzer const& action_analyzer,
				engine::ActionType action_type,
				engine::ActionChoices const& action_choices)
			{
				assert(!action_choices.Empty());

				int choices = action_choices.Size();

				if (choices == 1) {
					return 0;
				}

				if (action_type.IsChosenRandomly()) {
					int rnd = random_.GetRandom(choices);
					return action_choices.Get(rnd);
				}

				assert(action_type.IsChosenManually());

				int choice = select_.GetChoice(
					board, action_analyzer, action_type,
					policy::simulation::ChoiceGetter(choices)
				);
				assert(choice >= 0); // always return a valid choice

				return action_choices.Get(choice);
			}

		private:
			StaticConfigs::SimulationPhaseRandomActionPolicy random_;
			StaticConfigs::SimulationPhaseSelectActionPolicy select_;
		};
	}
}