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
			if (action_type != FlowControl::ActionType::kMainAction)
			{
				assert(action_choices.Size() > 0);
				if (action_choices.Size() == 1) return 0;
			}
			return callback_.ChooseAction(FlowControl::ActionType(action_type), action_choices);
		}
	}
}