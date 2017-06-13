#pragma once

#include "MCTS/Board.h"
#include "MCTS/simulation/Tree.h"

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

			int GetAction(ActionType action_type, int choices) {
				tree_.FillChoices(choices);
				int choice = Simulate(action_type, choices);
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
			int Simulate(ActionType action_type, int choices)
			{
				if (tree_.GetWhiteListCount() <= 0) return -1;

				if (action_type.IsChosenRandomly()) {
					int rnd = rand() % tree_.GetWhiteListCount();
					return (int)tree_.GetWhiteListItem((size_t)rnd);
				}

				// TODO: use value network to enhance simulation
				//return std::rand() % choices;
				std::vector<int> valid_choices;
				tree_.ForEachWhiteListItem([&](int choice) {
					valid_choices.push_back(choice);
					return true;
				});
				int rnd = rand() % valid_choices.size();
				return valid_choices[rnd];
			}

		private:
			Board saved_board_;
			Tree tree_;
		};
	}
}