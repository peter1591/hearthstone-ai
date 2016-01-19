#ifndef PLAYER_STAT_H
#define PLAYER_STAT_H

#include <string>
#include <sstream>
#include "weapon.h"
#include "hero-power.h"

class PlayerStat
{
	public:
		int hp;
		int armor;

		int crystals_total;
		int crystals_current;
		int crystals_locked;
		int crystals_locked_next_turn;

		Weapon weapon;
		HeroPower hero_power;

		std::string GetDebugString() const;
};

inline std::string PlayerStat::GetDebugString() const
{
	std::ostringstream oss;
	oss << "HP: " << this->hp << " + " << this->armor
		<< ", crystals: "
		<< this->crystals_current << "/" << this->crystals_total
		<< ", locked: "
		<< this->crystals_locked << "/" << this->crystals_locked_next_turn;
	return oss.str();
}

#endif
