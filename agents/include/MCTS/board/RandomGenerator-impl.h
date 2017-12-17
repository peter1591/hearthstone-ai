#pragma once

#include "MCTS/builder/TreeBuilder.h"

namespace mcts
{
	namespace board
	{
		inline int RandomGenerator::Get(int exclusive_max)
		{
			return callback_.ChooseAction(
				FlowControl::ActionType(FlowControl::ActionType::kRandom),
				FlowControl::ActionChoices(exclusive_max));
		}
	}
}