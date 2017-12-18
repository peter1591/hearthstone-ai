#pragma once

#include <vector>

#include "state/State.h"
#include "FlowControl/IActionParameterGetter.h"
#include "MCTS/Types.h"

namespace mcts
{
	class SOMCTS;

	namespace board
	{
		class ActionParameterGetter : public FlowControl::IActionParameterGetter
		{
		public:
			ActionParameterGetter(SOMCTS & callback) : callback_(callback) {}

			int GetNumber(FlowControl::ActionType::Types action_type, FlowControl::ActionChoices const& action_choices) final;

		private:
			SOMCTS & callback_;
		};
	}
}