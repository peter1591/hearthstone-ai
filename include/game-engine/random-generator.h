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

	public: // comparison
		bool operator==(const RandomGenerator &) const {
			// NOTE: boards only differ with the rand seed is considered the same
			//if (this->rand_seed != rhs.rand_seed) return false;
			return true;
		}

		bool operator!=(const RandomGenerator &rhs) const {
			return !(*this == rhs);
		}

	private:
		bool has_called;
};

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::RandomGenerator> {
		typedef GameEngine::RandomGenerator argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &) const {
			result_type result = 0;

			// NOTE: boards only differ with the rand seed is considered the same
			// GameEngine::hash_combine(result, hash<decltype(s.rand_seed)>()(s.rand_seed));

			return result;
		}
	};
}

#endif
