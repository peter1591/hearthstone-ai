#ifndef DECK_H
#define DECK_H

#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include <stdexcept>

#include "card.h"
#include "common.h"

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

	if (UNLIKELY(this->cards.empty())) {
		ret.MarkInvalid();
		return ret;
	}

	rand_idx = rand() % this->cards.size();

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
