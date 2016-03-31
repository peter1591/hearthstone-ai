#ifndef GAME_ENGINE_RANDOM_GENERATOR_H
#define GAME_ENGINE_RANDOM_GENERATOR_H

#include <cstdlib>
#include <random>
#include "common.h"

#include <functional>

namespace GameEngine {

class RandomGenerator
{
	friend std::hash<RandomGenerator>;

	public:
		RandomGenerator()
		{
		}

		void SetRandomSeed(unsigned int seed) const {
			srand(seed);
		}

		int GetRandom(int exclusive_max) {
			if (exclusive_max < 0) throw std::runtime_error("invalid argument");
			else if (exclusive_max == 0) return rand();
			else if (exclusive_max == 1) return 0;
			else {
				this->has_called = true;
				return rand() % exclusive_max;
			}
		}

		void ClearFlag_HasCalled() { this->has_called = false; }
		bool GetFlag_HasCalled() const { return this->has_called; }

	private:
		bool has_called;
};

} // namespace GameEngine

#endif
