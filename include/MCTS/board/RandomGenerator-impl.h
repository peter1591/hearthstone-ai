#pragma once

#include "MCTS/MCTS.h"

namespace mcts
{
	namespace board
	{
		inline int RandomGenerator::Get(int exclusive_max)
		{
			return mcts_.ChooseAction(ActionType(ActionType::kRandom), exclusive_max);
		}
	}
}