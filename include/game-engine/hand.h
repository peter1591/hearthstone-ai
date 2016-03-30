#ifndef GAME_ENGINE_HAND_H
#define GAME_ENGINE_HAND_H

#include <functional>
#include <vector>
#include <bitset>
#include "card.h"

namespace GameEngine {

class Hand
{
	friend std::hash<Hand>;

public:
	typedef size_t Locator;

public:
	Hand(RandomGenerator & random_generator);
	Hand(RandomGenerator & random_generator, Hand const& rhs);
	Hand & operator=(Hand const& rhs) = delete;
	Hand(RandomGenerator & random_generator, Hand && rhs);
	Hand & operator=(Hand && rhs);

public: // initializer
	void Initialize_AddHandCard(Card const& card);

public: // operations
	void DrawOneCardToHand();
	Card DrawOneCardAndDiscard();

	Card const& GetCard(Locator idx) const { return *(this->cards.begin() + idx); }
	void RemoveCard(Locator idx);
	size_t GetCount() const { return this->cards.size(); }

	void AddCardToDeck(Card const& card);
	bool IsDeckEmpty() const;

	bool operator==(const Hand &rhs) const;
	bool operator!=(const Hand &rhs) const { return !(*this == rhs); }

	std::string GetDebugString() const;

private:
	Card DrawFromDeck();

private:
	RandomGenerator & random_generator;

	std::vector<Card> cards;
	std::vector<Card> deck_cards;
};

} // namespace GameEngine

#endif
