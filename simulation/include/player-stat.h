#ifndef PLAYER_STAT_H
#define PLAYER_STAT_H

#include "weapon.h"
#include "hero-power.h"

struct PlayerStat
{
	int hp;
	int armor;

	int crystals_total;
	int crystals_current;
	int crystals_locked;
	int crystals_locked_next_turn;

	Weapon weapon;
	HeroPower hero_power;
};

#endif
