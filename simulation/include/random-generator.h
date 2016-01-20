#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <stdlib.h>

class RandomGenerator
{
	public:
		RandomGenerator() : rand_seedp(1) {
		}

		int GetRandom() {
			this->not_called = false;
			return rand_r(&this->rand_seedp);
		}

		void ClearFlags() {
			this->not_called = true;
		}

		void GetFlags(bool &not_called) {
			not_called = this->not_called;
		}

	private:
		unsigned int rand_seedp;
		bool not_called;
};

#endif
