#pragma once

#include "MCTS/builder/TreeBuilder.h"

namespace mcts
{
	namespace board
	{
		inline int RandomGenerator::Get(int exclusive_max)
		{
			return builder_.ChooseAction(ActionType(ActionType::kRandom), exclusive_max);
		}
	}
}