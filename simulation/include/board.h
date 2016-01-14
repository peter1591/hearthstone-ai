#ifndef _BOARD_H
#define _BOARD_H

#include <list>
#include <vector>

#include "card.h"
#include "player-stat.h"
#include "hidden-secret.h"
#include "hidden-hand-card.h"

class Board
{
	public:
		enum Turn {
			TURN_PLAYER, TURN_OPPONENT, TURN_RNG
		};

	public:
		PlayerStat player_stat;
		std::list<Card> player_secrets;
		std::list<Card> player_hand;
		std::list<Card> player_deck;
		std::list<Card> player_minions; // TODO: use a new 'Minion' type?

		PlayerStat opponent_stat;
		std::list<HiddenSecret> opponent_secrets;
		std::list<HiddenHandCard> opponent_hand;
		std::vector<Card> opponent_played_cards;

		Turn turn;
};

#endif
