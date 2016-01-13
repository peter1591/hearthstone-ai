#ifndef _PLAYER_STAT_H
#define _PLAYER_STAT_H

#include "card.h"
#include "hero-power.h"

struct PlayerStat
{
	int hp;
	int armor;

	int crystals_total;
	int crystals_current;
	int crystals_locked;
	int crystals_locked_next_turn;

	Card weapon;
	HeroPower hero_power;
};

#endif
