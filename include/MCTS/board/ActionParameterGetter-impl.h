#pragma once

#include "MCTS/board/ActionParameterGetter.h"
#include "MCTS/builder/TreeBuilder.h"

namespace mcts
{
	namespace board
	{
		inline int ActionParameterGetter::GetNumber(ActionType::Types action_type, int exclusive_max)
		{
			if (exclusive_max <= 0) return -1;
			return builder_.ChooseAction(ActionType(action_type), [exclusive_max]() {
				return ActionChoices(exclusive_max);
			});
		}

		inline int ActionParameterGetter::GetNumber(
			ActionType::Types action_type,
			ActionChoicesGetter const& action_choices_getter)
		{
			return builder_.ChooseAction(ActionType(action_type), action_choices_getter);
		}
	}
}