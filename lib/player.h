#ifndef _PLAYER_H
#define _PLAYER_H

#include "player-stat.h"
#include "card.h"

struct Player
{
	PlayerStat stat;

	Card[MAX_SECRETS] secrets;
	int secrets_len;

	Card[MAX_HAND] hand;
	int hand_len;

	Card[MAX_DECK] deck;
	int deck_len;
};

#endif
