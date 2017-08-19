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
			Simulation(state::PlayerSide side, std::mt19937 & rand, FlowControl::FlowContext & flow_context) :
				random_(rand), select_(side, rand), flow_context_(flow_context), action_analyzer_()
			{}

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
					int rnd = random_.GetRandom(choices);
					return action_choices.Get(rnd);
				}

				assert(action_type.IsChosenManually());

				int choice = select_.GetChoice(
					board, flow_context_, action_analyzer_, action_type,
					policy::simulation::ChoiceGetter(choices)
				);
				if (choice < 0) return -1;

				return action_choices.Get(choice);
			}

			void ReportInvalidAction() {
			}

		public:
			auto & GetFlowContext() { return flow_context_; }
			auto & GetActionAnalyzer() { return action_analyzer_; }

		private:
			StaticConfigs::SimulationPhaseRandomActionPolicy random_;
			StaticConfigs::SimulationPhaseSelectActionPolicy select_;
			FlowControl::FlowContext & flow_context_;
			board::BoardActionAnalyzer action_analyzer_;
		};
	}
}