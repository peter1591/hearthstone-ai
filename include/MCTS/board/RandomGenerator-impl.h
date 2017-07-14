#pragma once

#include "MCTS/builder/TreeBuilder.h"

namespace mcts
{
	namespace board
	{
		inline int RandomGenerator::Get(int exclusive_max)
		{
			return callback_.ChooseAction(ActionType(ActionType::kRandom), ActionChoices(exclusive_max));
		}
	}
}