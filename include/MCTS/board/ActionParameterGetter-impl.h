#pragma once

#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/SOMCTS.h"

namespace mcts
{
	namespace board
	{
		inline int ActionParameterGetter::GetNumber(ActionType::Types action_type, int exclusive_max)
		{
			if (exclusive_max <= 0) return -1;
			return callback_.ChooseAction(ActionType(action_type), ActionChoices(exclusive_max));
		}

		inline int ActionParameterGetter::GetNumber(
			ActionType::Types action_type,
			ActionChoices const& action_choices)
		{
			return callback_.ChooseAction(ActionType(action_type), action_choices);
		}
	}
}