#ifndef _OPPONENT_H
#define _OPPONENT_H

#include <list>
#include <vector>

#include "settings.h"
#include "hidden-secret.h"
#include "hidden-hand-card.h"
#include "player-stat.h"
#include "card.h"

// player with hidden information
class Opponent
{
	public:
		PlayerStat stat;

		std::list<HiddenSecret> secrets;
		std::list<HiddenHandCard> hand;
		std::vector<Card> played_cards;
};

#endif
