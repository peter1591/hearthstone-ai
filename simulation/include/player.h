#ifndef _PLAYER_H
#define _PLAYER_H

#include "settings.h"
#include "player-stat.h"
#include "card.h"

class Player
{
	public:
		PlayerStat stat;

		Card secrets[MAX_SECRETS];
		int secrets_len;

		Card hand[MAX_HAND];
		int hand_len;

		Card deck[MAX_DECK];
		int deck_len;

	public:
		void AddCardToDeck(const Card &card) {
			deck[deck_len] = card;
			deck_len++;
		}
};

#endif
