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

		void SetRandomSeed(unsigned int seed)
		{
			this->random_generator.seed(seed);
		}

		unsigned int GetRandom(unsigned int exclusive_max) {
			if (exclusive_max < 0) throw std::runtime_error("invalid argument");
			else if (exclusive_max == 0) return this->random_generator();
			else if (exclusive_max == 1) return 0;
			else {
				this->has_called = true;
				return this->random_generator() % exclusive_max;
			}
		}

		void ClearFlag_HasCalled() { this->has_called = false; }
		bool GetFlag_HasCalled() const { return this->has_called; }

	private:
		std::mt19937_64 random_generator;
		bool has_called;
};

} // namespace GameEngine

#endif
