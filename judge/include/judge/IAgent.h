#pragma once

#include <functional>
#include <random>

#include <state/State.h>

namespace judge
{
	class IAgent {
	public:
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
		virtual void Think(state::PlayerIdentifier side, std::function<state::State(int)> state_getter, std::mt19937 & random) = 0;

		virtual int GetAction(engine::ActionType::Types action_type, engine::ActionChoices action_choices) = 0;
	};
}
