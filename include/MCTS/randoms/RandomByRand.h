#pragma once

#include <cstdlib>

namespace mcts
{
	namespace randoms
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