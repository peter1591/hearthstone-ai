#ifndef _HAND_H
#define _HAND_H

#include <vector>
#include "card.h"

class Hand
{
	public:
		Hand();
		void AddCard(const Card &card);
		const std::vector<Card> &GetCards() const { return this->cards; }
		std::vector<Card> &GetCards() { return this->cards; }

	private:
		std::vector<Card> cards;
};

inline Hand::Hand()
{
	this->cards.reserve(10);
}

inline void Hand::AddCard(const Card &card)
{
	this->cards.push_back(card);
}

#endif
