#pragma once

#include "MCTS/board/Board.h"

namespace mcts
{
	namespace board
	{
		class ActionParameterGetter;
		class RandomGenerator;

		class BoardActionAnalyzer
		{
		private:
			enum MainActions
			{
				kActionPlayCard,
				kActionAttack,
				kActionHeroPower,
				kActionEndTurn,
				kActionMax // Should be at last
			};

		public:
			int GetActionsCount(Board const& board);
			Result ApplyAction(Board & board, int action, RandomGenerator & random, ActionParameterGetter & action_parameters);

		private:
			typedef Result(BoardActionAnalyzer::*CallbackFunc)(Board & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result PlayCard(Board & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result Attack(Board & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result HeroPower(Board & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result EndTurn(Board & board, RandomGenerator & random, ActionParameterGetter & action_parameters);

		private:
			std::vector<int> playable_cards_;

			std::vector<state::CardRef> attackers_;

			std::array<CallbackFunc, kActionMax> actions_;
		};
	}
}