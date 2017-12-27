#pragma once

#include <functional>
#include <random>

#include "state/State.h"
#include "engine/view/BoardRefView.h"

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

		virtual void Think(state::PlayerIdentifier side, engine::view::BoardRefView game_state , std::mt19937 & random) = 0;

		virtual int GetAction(engine::ActionType::Types action_type, engine::ActionChoices action_choices) = 0;
	};
}
