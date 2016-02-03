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

		void SetRandomSeed(unsigned int seed) {
			srand(seed);
		}

		int GetRandom() {
			return rand();
		}

		void ClearFlag_HasCalled() { this->has_called = false; }
		bool GetFlag_HasCalled() const { return this->has_called; }

	private:
		bool has_called;
};

} // namespace GameEngine

#endif
