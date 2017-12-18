#pragma once

#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/SOMCTS.h"

namespace mcts
{
	namespace board
	{
		inline int ActionParameterGetter::GetNumber(
			FlowControl::ActionType::Types action_type,
			FlowControl::ActionChoices const& action_choices)
		{
			return callback_.ChooseAction(FlowControl::ActionType(action_type), action_choices);
		}
	}
}