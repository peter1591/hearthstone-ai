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

		class RandomByXorShift
		{
		public:
			RandomByXorShift(uint32_t seed) : state_(seed) {
				// state should be filled with an non-zero
				if (state_ == 0) state_ = 1; // TODO: a better workaround?
			}

			int GetRandom(int exclusive_max) {
				uint32_t x = state_;
				x ^= x << 13;
				x ^= x >> 17;
				x ^= x << 5;
				state_ = x;

				// The random number is [1, 2^64-1]. Minus 1 to make it zero-based.
				--x;
				return (x - 1) % exclusive_max;
			}

		private:
			uint32_t state_;
		};

		using FastRandom = RandomByXorShift;
	}
}
