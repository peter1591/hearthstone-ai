#pragma once

#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/SOMCTS.h"

namespace mcts
{
	namespace board
	{
		inline int ActionParameterGetter::GetNumber(
			ActionType::Types action_type,
			ActionChoices const& action_choices)
		{
			if (action_type == ActionType::kMainAction) {
				return main_op_idx_;
			}
			else {
				return callback_.ChooseAction(ActionType(action_type), action_choices);
			}
		}
	}
}