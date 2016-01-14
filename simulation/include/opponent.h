#ifndef _OPPONENT_H
#define _OPPONENT_H

#include "settings.h"
#include "unknown-secret.h"
#include "player-stat.h"
#include "card.h"

// player with hidden information
struct Opponent
{
	PlayerStat stat;

	UnknownSecret secrets[MAX_SECRETS];
	int secrets_len;

	// TODO: record info for partially-known information
	int hands_cards_len;

	Card played_cards[MAX_DECK]; // FIXME: there should be no limit on the number of played cards
	int played_cards_len;
};

#endif
