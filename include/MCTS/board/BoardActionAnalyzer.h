#pragma once

#include "MCTS/Result.h"

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
			int GetActionsCount(state::State const& board);
			Result ApplyAction(state::State & board, int action, RandomGenerator & random, ActionParameterGetter & action_parameters);

		private:
			typedef Result(BoardActionAnalyzer::*CallbackFunc)(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result PlayCard(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result Attack(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result HeroPower(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);
			Result EndTurn(state::State & board, RandomGenerator & random, ActionParameterGetter & action_parameters);

		private:
			std::vector<int> playable_cards_;

			std::vector<state::CardRef> attackers_;

			std::array<CallbackFunc, kActionMax> actions_;
		};
	}
}