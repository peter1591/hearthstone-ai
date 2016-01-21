#ifndef DECK_DATABASE_H
#define DECK_DATABASE_H

#include "card.h"

namespace GameEngine {

class DeckDatabase
{
	public:
		DeckDatabase();

		Card GetCard(int card_id) const;
};

} // namespace GameEngine

#endif
