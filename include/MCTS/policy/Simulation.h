#pragma once

#include <random>
#include "MCTS/board/Board.h"

namespace mcts
{
	namespace policy
	{
		namespace simulation
		{
			class ChoiceGetter
			{
			public:
				ChoiceGetter(int choices) : choices_(choices) {}

				size_t Size() const { return (size_t)choices_; }

				int Get(size_t idx) const { return (int)idx; }

				template <typename Functor>
				void ForEachChoice(Functor&& functor) const {
					for (int i = 0; i < choices_; ++i) {
						if (!functor(i)) return;
					}
				}

			private:
				int choices_;
			};

			class RandomPolicy
			{
			public:
				static int GetChoice(ChoiceGetter const& choice_getter, board::Board const& board)
				{
					// TODO: use value network to enhance simulation
					size_t count = choice_getter.Size();
					assert(count > 0);
					size_t idx = 0;
					size_t rand_idx = (size_t)(std::rand() % count);
					int result = -1;
					choice_getter.ForEachChoice([&](int choice) {
						if (idx == rand_idx) {
							result = choice;
							return false;
						}
						++idx;
						return true;
					});
					assert([&]() {
						int result2 = choice_getter.Get(rand_idx);
						return result == result2;
					}());
					return result;
				}
			};

			class StateValueFunction
			{
			public:
				double static GetStateValue(board::Board const& board) {
					return 0.0; // TODO
				}
			};

			class HeuristicPolicy
			{
			public:
				HeuristicPolicy(std::mt19937 & rand) : rand_(rand) {

				}

				void StartNewAction() {

				}

				int GetChoice(
					board::Board const& board,
					board::BoardActionAnalyzer const& action_analyzer,
					ActionType action_type,
					ChoiceGetter const& choice_getter)
				{
					if (action_type == ActionType::kMainAction) {
						return GetChoiceForMainAction(
							board, action_analyzer, choice_getter);
					}

					// TODO: use value network to enhance simulation
					size_t count = choice_getter.Size();
					assert(count > 0);
					size_t idx = 0;
					size_t rand_idx = (size_t)(std::rand() % count);
					int result = -1;
					choice_getter.ForEachChoice([&](int choice) {
						if (idx == rand_idx) {
							result = choice;
							return false;
						}
						++idx;
						return true;
					});
					assert([&]() {
						int result2 = choice_getter.Get(rand_idx);
						return result == result2;
					}());
					return result;
				}

			private:
				int GetChoiceForMainAction(
					board::Board const& board,
					board::BoardActionAnalyzer const& action_analyzer,
					ChoiceGetter const& choice_getter)
				{
					int choice = -1;

					// play card if it's a viable option
					action_analyzer.ForEachMainOp([&](size_t idx, board::BoardActionAnalyzer::OpType op) {
						if (op == board::BoardActionAnalyzer::kPlayCard) {
							choice = (int)idx;
							return false;
						}
						return true;
					});

					if (choice >= 0) {
						assert(action_analyzer.GetMainOpType((size_t)choice) == board::BoardActionAnalyzer::kPlayCard);
						return choice;
					}

					// choose end-turn only when it is the only option
					size_t count = choice_getter.Size();
					assert(count > 0);
					if (count == 1) {
						// should be end-turn
						assert(action_analyzer.GetMainOpType((size_t)0) == board::BoardActionAnalyzer::kEndTurn);
						return 0;
					}

					// rule out the end-turn action
					assert(action_analyzer.GetMainOpType(count - 1) == board::BoardActionAnalyzer::kEndTurn);
					--count;
					assert(count > 0);

					// otherwise, choose randomly
					size_t rand_idx = (size_t)(std::rand() % count);
					return choice_getter.Get(rand_idx);
				}

			private:
				std::mt19937 rand_;
			};
		}
	}
}