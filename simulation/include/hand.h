#ifndef _HAND_H
#define _HAND_H

#include <list>
#include "card.h"

class Hand
{
	public:
		void AddCard(const Card &card);
		const std::list<Card> &GetCards() const;

	private:
		std::list<Card> cards;
};

inline void Hand::AddCard(const Card &card)
{
	this->cards.push_back(card);
}

inline const std::list<Card> &Hand::GetCards() const
{
	return this->cards;
}

#endif
