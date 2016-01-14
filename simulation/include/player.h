#ifndef _PLAYER_H
#define _PLAYER_H

#include <list>

#include "settings.h"
#include "player-stat.h"
#include "card.h"

class Player
{
	public:
		PlayerStat stat;

		std::list<Card> secrets;
		std::list<Card> hand;
		std::list<Card> deck;
};

#endif
