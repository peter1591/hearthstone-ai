#pragma once

#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/builder/TreeBuilder.h"

namespace mcts
{
	namespace board
	{
		inline int ActionParameterGetter::GetNumber(ActionType::Types action_type, int exclusive_max)
		{
			return builder_.ChooseAction(ActionType(action_type), ActionChoices(exclusive_max));
		}
	}
}