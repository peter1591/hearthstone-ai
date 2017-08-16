#pragma once

#include <stdlib.h>
#include <cstdlib>
#include <random>

namespace mcts
{
	namespace policy
	{
		class RandomByMt19937
		{
		public:
			RandomByMt19937(std::mt19937 & rand) : inst_(rand) {}

			int GetRandom(int exclusive_max) {
				return inst_() % exclusive_max;
			}

		private:
			std::mt19937 & inst_;
		};

		class RandomByRand
		{
		public:
			RandomByRand(int seed) : state_((unsigned int)seed) {}

			int GetRandom(int exclusive_max) {
				return rand_r(&state_) % exclusive_max;
			}

		private:
			unsigned int state_;
		};
	}
}
