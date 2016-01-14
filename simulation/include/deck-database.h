#ifndef _DECK_DATABASE_H
#define _DECK_DATABASE_H

#include "card.h"

class DeckDatabase
{
	public:
		DeckDatabase();

		Card GetCard(int card_id) const;
};

#endif
