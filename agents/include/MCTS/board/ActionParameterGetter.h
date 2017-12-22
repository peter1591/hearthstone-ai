#pragma once

#include <vector>

#include "state/State.h"
#include "engine/IActionParameterGetter.h"
#include "MCTS/Types.h"

namespace mcts
{
	class SOMCTS;

	namespace board
	{
		class ActionParameterGetter : public engine::IActionParameterGetter
		{
		public:
			ActionParameterGetter(SOMCTS & callback) : callback_(callback) {}

			int GetNumber(engine::ActionType::Types action_type, engine::ActionChoices const& action_choices) final;

		private:
			SOMCTS & callback_;
		};
	}
}