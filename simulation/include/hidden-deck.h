#ifndef HIDDEN_DECK_H
#define HIDDEN_DECK_H

#include <sstream>
#include <string>
#include <vector>
#include "card.h"

class HiddenDeck
{
	public:
		HiddenDeck() : deck_card_count(0) {}

		std::vector<Card> played_cards;

		void Set(int size) { this->deck_card_count = size; }
		int GetCardCount() const { return this->deck_card_count; }

		void AddCard(const Card &) {
			++this->deck_card_count;
			// TODO: record the added card
		}

		Card Draw() {
			Card card;
			card.type = Card::TYPE_MINION;
			card.id = 333;
			card.data.minion.cost = 3;
			card.data.minion.attack = 3;
			card.data.minion.hp = 3;

			--this->deck_card_count;
			return card; // TODO
		}

		// TODO: we can guess the opponent deck type

		std::string GetDebugString() const;

	private:
		int deck_card_count;
};

inline std::string HiddenDeck::GetDebugString() const
{
	std::ostringstream oss;
	oss << "count: " << this->deck_card_count;
	return oss.str();
}

#endif
