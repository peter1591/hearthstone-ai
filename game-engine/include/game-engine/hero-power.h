#ifndef GAME_ENGINE_HERO_POWER_H
#define GAME_ENGINE_HERO_POWER_H

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

#endif
