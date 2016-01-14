#include "deck-database.h"

DeckDatabase::DeckDatabase()
{
	// TODO
}

Card DeckDatabase::GetCard(int card_id) const
{
	Card card;

	if (card_id == 111) {
		card.id = card_id;
		card.type = Card::MINION;
		card.data.minion.cost = 1;
		card.data.minion.attack = 1;
		card.data.minion.hp = 1;
	}
	else if (card_id == 211) {
		card.id = card_id;
		card.type = Card::MINION;
		card.data.minion.cost = 2;
		card.data.minion.attack = 1;
		card.data.minion.hp = 1;
	}
	else if (card_id == 213) {
		card.id = card_id;
		card.type = Card::MINION;
		card.data.minion.cost = 2;
		card.data.minion.attack = 1;
		card.data.minion.hp = 3;
	}
	else if (card_id == 222) {
		card.id = card_id;
		card.type = Card::MINION;
		card.data.minion.cost = 2;
		card.data.minion.attack = 2;
		card.data.minion.hp = 2;
	}
	else if (card_id == 231) {
		card.id = card_id;
		card.type = Card::MINION;
		card.data.minion.cost = 2;
		card.data.minion.attack = 3;
		card.data.minion.hp = 1;
	}
	else {
		card.id = 0;
	}

	return card;
}
