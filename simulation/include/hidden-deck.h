#ifndef HIDDEN_DECK_H
#define HIDDEN_DECK_H

#include <vector>
#include "card.h"

class HiddenDeck
{
	public:
		HiddenDeck();

		std::vector<Card> played_cards;
		// TODO: we can guess the opponent deck type
};

inline HiddenDeck::HiddenDeck()
{
	this->played_cards.reserve(30);
}

#endif
