#ifndef OPPONENT_CARDS_H
#define OPPONENT_CARDS_H

#include <sstream>
#include <string>
#include <vector>
#include "card.h"

class OpponentCards
{
	public:
		OpponentCards() : deck_card_count(0), hand_card_count(0) {}

		std::vector<Card> played_cards;

		void Set(int deck_size) { this->deck_card_count = deck_size; }
		int GetHandCount() const { return this->hand_card_count; }
		int GetDeckCount() const { return this->deck_card_count; }

		void AddCardToDeck(const Card &) {
			++this->deck_card_count;
			// TODO: record the added card
		}

		Card DrawFromHand() {
			Card card;
			card.type = Card::TYPE_MINION;
			card.id = 333;
			card.data.minion.cost = 3;
			card.data.minion.attack = 3;
			card.data.minion.hp = 3;

			--this->hand_card_count;
			return card; // TODO
		}

		void DrawFromDeckToHand() {
			--this->deck_card_count;
			++this->hand_card_count;
		}

		// TODO: we can guess the opponent deck type

		std::string GetDebugString() const;

	private:
		int deck_card_count;
		int hand_card_count;
};

inline std::string OpponentCards::GetDebugString() const
{
	std::ostringstream oss;
	oss << "Hand: " << this->hand_card_count
		<< ", Deck: " << this->deck_card_count;
	return oss.str();
}

#endif
