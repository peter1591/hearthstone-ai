#pragma once

namespace state {
	class IRandomGenerator
	{
	public:
		IRandomGenerator() {}
		virtual ~IRandomGenerator() {}

		IRandomGenerator(IRandomGenerator const&) = delete;
		IRandomGenerator & operator=(IRandomGenerator const&) = delete;

		virtual int Get(int exclusive_max) = 0;
		virtual size_t Get(size_t exclusive_max) = 0;

		// @param min Inclusive minimum
		// @param max Inclusive maximum
		virtual int Get(int min, int max) = 0;
	};
}