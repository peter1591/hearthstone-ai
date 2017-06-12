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
				choice_white_list_.Clear();
			}

			int GetAction(ActionType action_type, int choices) {
				choice_white_list_.FillChoices(choices);
				int choice = Simulate(action_type, choices);
				choice_white_list_.ApplyChoice(choice);
				return choice;
			}

			void ReportInvalidAction() {
				choice_white_list_.ReportInvalidChoice();
			}

			// @return saved board
			Board const& RestartAction() {
				// Use a white-list-tree to record all viable (sub-)actions
				choice_white_list_.Restart();
				return saved_board_;
			}

		private:
			int Simulate(ActionType action_type, int choices)
			{
				if (choice_white_list_.GetWhiteListCount() <= 0) return -1;

				std::vector<int> valid_choices;
				choice_white_list_.ForEachWhiteListItem([&](int choice) {
					valid_choices.push_back(choice);
					return false; // early stop
				});

				if (action_type.IsChosenRandomly()) {
					int rnd = rand() % valid_choices.size();
					return valid_choices[rnd];
				}

				int rnd = rand() % valid_choices.size();
				return valid_choices[rnd];
				// TODO: use value network to enhance simulation
				//return std::rand() % choices;
			}

		private:
			Board saved_board_;
			Tree choice_white_list_;
		};
	}
}