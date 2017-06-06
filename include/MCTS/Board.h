#pragma once

#include "state/State.h"
#include "FlowControl/FlowController.h"

namespace mcts
{
	using Result = FlowControl::Result;

	class ActionParameterGetter;
	class RandomGenerator;

	class Board
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
		Board() {}
		Board(state::State const& state) : state_(state) {}

		int GetActionsCount();
		Result ApplyAction(int action, RandomGenerator & random, ActionParameterGetter & action_parameters);
		state::State const& GetState() const { return state_; }

	private:
		typedef Result (Board::*CallbackFunc)(RandomGenerator & random, ActionParameterGetter & action_parameters);
		Result PlayCard(RandomGenerator & random, ActionParameterGetter & action_parameters);
		Result Attack(RandomGenerator & random, ActionParameterGetter & action_parameters);
		Result HeroPower(RandomGenerator & random, ActionParameterGetter & action_parameters);
		Result EndTurn(RandomGenerator & random, ActionParameterGetter & action_parameters);

		state::CardRef UserChooseSideCharacter(state::PlayerIdentifier player_id, ActionParameterGetter & action_parameters);

	private:
		state::State state_;
		
		std::array<CallbackFunc, kActionMax> actions_;
	};
}