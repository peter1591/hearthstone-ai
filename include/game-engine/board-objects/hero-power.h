#ifndef GAME_ENGINE_BOARD_OBJECTS_HERO_POWER_H
#define GAME_ENGINE_BOARD_OBJECTS_HERO_POWER_H

#include <functional>
#include "game-engine/common.h"

namespace GameEngine {
	struct HeroPower
	{
		// TODO

		bool operator==(const HeroPower &) const
		{
			return true;
		}

		bool operator!=(const HeroPower &rhs) const
		{
			return !(*this == rhs);
		}
	};

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::HeroPower> {
		typedef GameEngine::HeroPower argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &) const {
			result_type result = 0;

			return result;
		}
	};

}

#endif
