#pragma once

#include "MCTS/Board.h"
#include "MCTS/detail/ChoiceWhiteList.h"

namespace mcts
{
	namespace stages
	{
		class Simulation
		{
		public:
			void StartEpisode() {}

			void StartNewAction(Board const& board) {
				saved_board_ = board;
				choice_white_list_.Clear();
			}

			int GetAction(int choices, bool random) {
				choice_white_list_.FillChoices(choices);
				int choice = Simulate(choices, random);
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
			int Simulate(int choices, bool random)
			{
				if (choice_white_list_.GetWhiteListCount() <= 0) return -1;

				int final_choice = 0;
				choice_white_list_.ForEachWhiteListItem([&](int choice) {
					final_choice = choice;
					return false; // early stop
				});
				return final_choice;
				// TODO: use value network to enhance simulation
				//return std::rand() % choices;
			}

		private:
			Board saved_board_;
			detail::ChoiceWhiteList choice_white_list_;
		};
	}
}