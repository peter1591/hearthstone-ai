#pragma once

#include <random>

namespace GameEngine
{
	// use a stronger random sequence to generate the random seeds
	class RandomSeedManager
	{
	public:
		void Initialize(int rand_seed)
		{
			this->random_generator.seed(rand_seed);
		}

		int GetNextRandomSeed()
		{
			return this->random_generator();
		}

	private:
		std::mt19937_64 random_generator;
	};
} // namespace GameEngine