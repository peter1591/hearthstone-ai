#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <stdlib.h>

class RandomGenerator
{
	public:
		RandomGenerator() : rand_seedp(1) {
		}

		int GetRandom() {
			has_called = true;
			return rand_r(&this->rand_seedp);
		}

		void ClearFlags() {
			has_called = false;
		}

		void GetFlags(bool &has_called) {
			has_called = RandomGenerator::has_called;
		}

	private:
		unsigned int rand_seedp;
		bool has_called;
};

#endif
