#ifndef GAME_ENGINE_HAND_H
#define GAME_ENGINE_HAND_H

#include <functional>
#include <vector>
#include <bitset>

#include "hand-card.h"
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

	bool operator==(const Hand &rhs) const;
	bool operator!=(const Hand &rhs) const;

public: // initializer
	void Initialize_AddPlayerHandCard(Card const & card);

public: // operations
	bool HasCardToDraw() const;
	void DrawOneCardToHand();
	Card DrawOneCardAndDiscard();

	Card const& GetCard(Locator idx) const;
	void RemoveCard(Locator idx);
	size_t GetCount() const;

	void AddCardToDeck(Card const& card);

	std::string GetDebugString() const;

private:
	Card DrawOneCardFromDeck();

private:
	RandomGenerator & random_generator;

	std::vector<HandCard> cards;
	std::vector<Card> deck_cards;
};

} // namespace GameEngine

#endif
