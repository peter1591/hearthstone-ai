#ifndef GAME_ENGINE_HAND_H
#define GAME_ENGINE_HAND_H

#include <vector>
#include "card.h"

namespace GameEngine {

class Hand
{
	public:
		Hand();
		void AddCard(const Card &card);
		const std::vector<Card> &GetCards() const { return this->cards; }
		int GetCountByCardType(Card::Type t) const;
		void RemoveCard(int idx);

	private:
		std::vector<Card> cards;
		int count_by_type[Card::TYPE_MAX];
};

inline Hand::Hand()
{
	this->cards.reserve(10);
	for (int i=0; i<Card::TYPE_MAX; ++i) {
		this->count_by_type[i] = 0;
	}
}

inline void Hand::AddCard(const Card &card)
{
	this->cards.push_back(card);
	this->count_by_type[card.type]++;
}

inline int Hand::GetCountByCardType(Card::Type t) const
{
	return this->count_by_type[t];
}

inline void Hand::RemoveCard(int idx)
{
	std::vector<Card>::iterator it = this->cards.begin() + idx;
	this->count_by_type[it->type]--;
	this->cards.erase(it);
}

} // namespace GameEngine

#endif
