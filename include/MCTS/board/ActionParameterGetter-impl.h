#pragma once

#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/MCTS.h"

namespace mcts
{
	namespace board
	{
		inline int ActionParameterGetter::GetNumber(ActionType::Types action_type, int exclusive_max)
		{
			return mcts_.ChooseAction(ActionType(action_type), exclusive_max);
		}
	}
}