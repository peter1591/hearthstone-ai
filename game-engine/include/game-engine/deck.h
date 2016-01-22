#ifndef GAME_ENGINE_DECK_H
#define GAME_ENGINE_DECK_H

#include <iostream>
#include <algorithm>
#include <utility>
#include <vector>
#include <stdexcept>

#include "card.h"
#include "common.h"
#include "random-generator.h"

namespace GameEngine {

class Deck
{
	public:
		Deck(RandomGenerator *random_generator);

	private:
		Deck(const Deck &);
		Deck &operator=(const Deck &);
	public:
		void Assign(const Deck &rhs, RandomGenerator *random_generator) {
			this->cards = rhs.cards;
			this->random_generator = random_generator;
		}

	public:
		// Add a card to deck
		void AddCard(const Card &card);

		Card Draw();

		const std::vector<Card>& GetCards() const;

		bool operator==(const Deck &rhs) const;
		bool operator!=(const Deck &rhs) const {
			return !(*this == rhs);
		}

	private:
		std::vector<Card> cards;
		RandomGenerator *random_generator;
};

inline Deck::Deck(RandomGenerator *random_generator) :
	random_generator(random_generator)
{
#ifdef DEBUG
	if (random_generator == nullptr) {
		throw std::runtime_error("Deck should be initialized with a valid random generator");
	}
#endif

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
		rand_idx = this->random_generator->GetRandom() % card_count;
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

inline bool Deck::operator==(const Deck &rhs) const
{
	if (this->cards != rhs.cards) return false;
	return true;
}

} // namespace GameEngine

#endif
