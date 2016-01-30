#ifndef GAME_ENGINE_COMMON_H
#define GAME_ENGINE_COMMON_H

#include <functional>

#ifndef _MSC_VER
#define LIKELY(expression) (__builtin_expect(!!(expression), 1))
#define UNLIKELY(expression) (__builtin_expect(!!(expression), 0))

#else
#define LIKELY(expression) (expression)
#define UNLIKELY(expression) (expression)
#endif

namespace GameEngine
{
	inline void hash_combine(std::size_t &seed, std::size_t new_hash)
	{
		seed ^= new_hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
}

#endif
