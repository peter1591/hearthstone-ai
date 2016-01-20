#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <stdlib.h>

class RandomGenerator
{
	public:
		static RandomGenerator& GetInstance() {
			static RandomGenerator instance;
			return instance;
		}

		int GetRandom() {
			this->has_called = true; return rand();
		}

		void ClearFlags() {
			this->has_called = false;
		}

		void GetFlags(bool &has_called) {
			has_called = RandomGenerator::has_called;
		}

	private:
		RandomGenerator() {}

	private:
		// TODO: should be thread safe by introducing a thread-local variable
		bool has_called;
};

#endif
