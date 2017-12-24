#pragma once

#include <cassert>

namespace engine {
	namespace FlowControl {
		class IRandomGenerator
		{
		public:
			IRandomGenerator() {}
			virtual ~IRandomGenerator() {}

			IRandomGenerator(IRandomGenerator const&) = delete;
			IRandomGenerator & operator=(IRandomGenerator const&) = delete;

			virtual int Get(int exclusive_max) = 0;

			size_t Get(size_t exclusive_max) { return (size_t)Get((int)exclusive_max); }

			// @param min Inclusive minimum
			// @param max Inclusive maximum
			int Get(int min, int max) {
				assert(max >= min);
				return min + Get(max - min + 1);
			}
		};
	}
}