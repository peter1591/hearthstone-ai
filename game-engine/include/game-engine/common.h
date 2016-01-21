#ifndef GAME_ENGINE_COMMON_H
#define GAME_ENGINE_COMMON_H

#define LIKELY(expression) (__builtin_expect(!!(expression), 1))
#define UNLIKELY(expression) (__builtin_expect(!!(expression), 0))

#endif
