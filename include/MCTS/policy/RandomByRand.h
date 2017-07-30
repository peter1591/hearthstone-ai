#pragma once

#include <cstdlib>
#include <random>

namespace mcts
{
	namespace policy
	{
		class RandomByMt19937
		{
		public:
			RandomByMt19937(int seed) : inst_(seed) {}

			int GetRandom(int exclusive_max) {
				return inst_() % exclusive_max;
			}

		private:
			std::mt19937 inst_;
		};
	}
}