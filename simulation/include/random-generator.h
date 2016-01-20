#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <stdlib.h>

class RandomGenerator
{
	public:
		static int GetRandom() {
			//has_called = true;
			return rand();
		}

		static void ClearFlags() {
			has_called = false;
		}

		static void GetFlags(bool &has_called) {
			has_called = RandomGenerator::has_called;
		}

	private:
		// TODO: should be thread safe by introducing a thread-local variable
		static bool has_called;
};

#endif
