#pragma once

#include <functional>
#include <random>

#include <state/State.h>

namespace judge
{
	class IAgent {
	public:
		using StartingStateGetter = std::function<state::State()>;

		enum MainActions
		{
			kActionPlayCard,
			kActionAttack,
			kActionHeroPower,
			kActionEndTurn
		};

		virtual ~IAgent() {}

		// TODO: The underlying truth state is passed. It means the competitor can acquire
		// hidden information (e.g., opponents hand cards).
		// should pass StartingStateGetter instead?
		virtual void Think(state::State const& state, std::mt19937 & random) = 0;

		virtual int GetAction(mcts::ActionType::Types action_type, mcts::board::ActionChoices action_choices) = 0;
	};
}