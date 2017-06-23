#pragma once

#include <cstdlib>

namespace mcts
{
	namespace policy
	{
		class RandomByRand
		{
		public:
			static int GetRandom(int exclusive_max)
			{
				return std::rand() % exclusive_max;
			}
		};
	}
}