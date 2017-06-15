#pragma once

#include "MCTS/Board.h"
#include "MCTS/Config.h"
#include "MCTS/simulation/Tree.h"
#include "MCTS/simulation/policy/PolicyBase.h"

namespace mcts
{
	namespace simulation
	{
		class Simulation
		{
		public:
			void StartEpisode() {}

			void StartNewAction(Board const& board) {
				saved_board_ = board;
				tree_.Clear();
			}

			int GetAction(Board const& board, ActionType action_type, int choices) {
				tree_.FillChoices(choices);
				int choice = Simulate(board, action_type, choices);
				tree_.ApplyChoice(choice);
				return choice;
			}

			void ReportInvalidAction() {
				tree_.ReportInvalidChoice();
			}

			// @return saved board
			Board const& RestartAction() {
				// Use a white-list-tree to record all viable (sub-)actions
				tree_.Restart();
				return saved_board_;
			}

		private:
			int Simulate(Board const& board, ActionType action_type, int choices) const
			{
				if (tree_.GetWhiteListCount() <= 0) return -1;

				if (action_type.IsChosenRandomly()) {
					int rnd = StaticConfigs::SimulationPhaseRandomActionPolicy::GetRandom((int)tree_.GetWhiteListCount());
					return (int)tree_.GetWhiteListItem((size_t)rnd);
				}

				int choice = StaticConfigs::SimulationPhaseSelectActionPolicy::GetChoice(
					policy::PolicyBase::ChoiceGetter(tree_), board
				);

				return choice;
			}

		private:
			Board saved_board_;
			Tree tree_;
		};
	}
}