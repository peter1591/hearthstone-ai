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
			if (action_type == FlowControl::ActionType::kMainAction) {
				return main_op_idx_;
			}
			else {
				return callback_.ChooseAction(FlowControl::ActionType(action_type), action_choices);
			}
		}
	}
}