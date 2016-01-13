#ifndef _OPPONENT_H
#define _OPPONENT_H

#include "player-stat.h"
#include "card.h"

// player with hidden information
struct Opponent
{
	PlayerStat stat;

	UnknownSecret[MAX_SECRETS] secrets;
	int secrets_len;

	Card[MAX_DECK] played_cards;
	int played_cards_len;
};

#endif
