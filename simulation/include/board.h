#ifndef _BOARD_H
#define _BOARD_H

#include <list>
#include <vector>

#include "card.h"
#include "deck.h"
#include "hand.h"
#include "secrets.h"
#include "minions.h"
#include "player-stat.h"
#include "hidden-deck.h"
#include "hidden-secrets.h"
#include "hidden-hand.h"

class Board
{
	public:
		enum Turn {
			TURN_PLAYER, TURN_OPPONENT // TODO: MSTC might need TURN_RNG to support random node
		};

	public:
		Turn turn;

		PlayerStat player_stat;
		Secrets player_secrets;
		Hand player_hand;
		Deck player_deck;
		Minions player_minions;

		PlayerStat opponent_stat;
		HiddenSecrets opponent_secrets;
		HiddenHand opponent_hand;
		HiddenDeck opponent_deck;
		Minions opponent_minions;
		std::vector<Card> opponent_played_cards;

	public:
		void PlayCard(const Card &card) const;
		void PrintBoard() const; // TODO: debug only
};

#endif
