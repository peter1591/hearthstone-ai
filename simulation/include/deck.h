#ifndef DECK_H
#define DECK_H

#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include <stdexcept>

#include "card.h"
#include "common.h"
#include "random-generator.h"

class Deck
{
	public:
		Deck();

		// Add a card to deck
		void AddCard(const Card &card);

		Card Draw();

		const std::vector<Card>& GetCards() const;

	private:
		std::vector<Card> cards;
};

inline Deck::Deck()
{
	this->cards.reserve(36);
}

inline void Deck::AddCard(const Card &card)
{
	this->cards.push_back(card);
}

inline Card Deck::Draw()
{
	int rand_idx;
	Card ret;
	size_t card_count = this->cards.size();

	if (UNLIKELY(card_count == 0)) {
		ret.MarkInvalid();
		return ret;
	}

	if (UNLIKELY(card_count == 1)) {
		rand_idx = 0;
	} else {
		rand_idx = RandomGenerator::GetInstance().GetRandom() % card_count;
	}

	ret = this->cards[rand_idx];

	this->cards[rand_idx] = this->cards.back();
	this->cards.pop_back();

	return ret;
}

inline const std::vector<Card>& Deck::GetCards() const
{
	return this->cards;
}

#endif
