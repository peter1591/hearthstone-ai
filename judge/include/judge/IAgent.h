#pragma once

#include <functional>

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
		virtual void Think(state::State const& state, int seed, std::function<bool(uint64_t)> cb) = 0;

		virtual int GetMainAction() = 0;

		// TODO: Move BoardActionAnalyzer out of 'mcts' namespace
		virtual mcts::board::BoardActionAnalyzer GetActionApplier() = 0;

		virtual int GetSubAction(mcts::ActionType::Types action_type, mcts::board::ActionChoices action_choices) = 0;
	};

}