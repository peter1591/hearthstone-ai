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
			Simulation(state::PlayerSide side, std::mt19937 & rand) :
				random_(rand), select_(side, rand), action_analyzer_()
			{}

			void StartNewAction() {
			}

			Result CutoffCheck(board::Board const& board)
			{
				using Policy = std::decay_t<decltype(select_)>;

				if constexpr (Policy::kEnableCutoff) {
					return select_.GetCutoffResult(board);
				}
				else {
					return Result::kResultNotDetermined;
				}
			}

			int ChooseAction(
				board::Board const& board,
				ActionType action_type,
				board::ActionChoices const& action_choices)
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
					board, action_analyzer_, action_type,
					policy::simulation::ChoiceGetter(choices)
				);
				assert(choice >= 0); // always return a valid choice

				return action_choices.Get(choice);
			}

		public:
			auto & GetActionAnalyzer() { return action_analyzer_; }

		private:
			StaticConfigs::SimulationPhaseRandomActionPolicy random_;
			StaticConfigs::SimulationPhaseSelectActionPolicy select_;
			board::BoardActionAnalyzer action_analyzer_;
		};
	}
}