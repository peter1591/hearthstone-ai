#ifndef GAME_ENGINE_RANDOM_GENERATOR_H
#define GAME_ENGINE_RANDOM_GENERATOR_H

#include <stdlib.h>
#include "common.h"

#include <functional>

namespace GameEngine {

class RandomGenerator
{
	friend std::hash<RandomGenerator>;

	public:
		RandomGenerator() : rand_seed(1) {
		}

		void SetRandomSeed(unsigned int seed) {
			this->rand_seed = seed;
		}

		unsigned int GetRandomSeed() const {
			return this->rand_seed;
		}

		int GetRandom() {
			this->not_called = false;
			return rand_r(&this->rand_seed);
		}

		void ClearFlags() {
			this->not_called = true;
		}

		void GetFlags(bool &not_called) {
			not_called = this->not_called;
		}

	public: // comparison
		bool operator==(const RandomGenerator &rhs) const {
			// NOTE: boards only differ with the rand seed is considered the same
			//if (this->rand_seed != rhs.rand_seed) return false;
			if (this->not_called != rhs.not_called) return false;
			return true;
		}

		bool operator!=(const RandomGenerator &rhs) const {
			return !(*this == rhs);
		}

	private:
		unsigned int rand_seed;
		bool not_called;
};

} // namespace GameEngine

namespace std {
	template <> struct hash<GameEngine::RandomGenerator> {
		typedef GameEngine::RandomGenerator argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			// NOTE: boards only differ with the rand seed is considered the same
			// GameEngine::hash_combine(result, hash<decltype(s.rand_seed)>()(s.rand_seed));

			GameEngine::hash_combine(result, hash<decltype(s.not_called)>()(s.not_called));

			return result;
		}
	};
}

#endif
