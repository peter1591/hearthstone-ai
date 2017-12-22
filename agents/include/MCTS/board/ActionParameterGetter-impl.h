#pragma once

#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/SOMCTS.h"

namespace mcts
{
	namespace board
	{
		inline int ActionParameterGetter::GetNumber(
			engine::ActionType::Types action_type,
			engine::ActionChoices const& action_choices)
		{
			if (action_type != engine::ActionType::kMainAction)
			{
				assert(action_choices.Size() > 0);
				if (action_choices.Size() == 1) return 0;
			}
			return callback_.ChooseAction(engine::ActionType(action_type), action_choices);
		}
	}
}