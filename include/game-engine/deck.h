#ifndef GAME_ENGINE_DECK_H
#define GAME_ENGINE_DECK_H

#include <functional>
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
	friend std::hash<Deck>;

	public:
		Deck(RandomGenerator & random_generator);

		Deck(Deck const& rhs) = delete;
		Deck & operator=(Deck const& rhs);

		Deck(Deck && rhs) = delete;
		Deck & operator=(Deck && rhs);

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
		RandomGenerator & random_generator;
};

inline Deck::Deck(RandomGenerator & random_generator) :
	random_generator(random_generator)
{
	this->cards.reserve(36);
}

inline Deck & Deck::operator=(Deck const& rhs) {
	this->cards = rhs.cards;
	return *this;
}

inline Deck & Deck::operator=(Deck && rhs) {
	this->cards = std::move(rhs.cards);
	return *this;
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
		rand_idx = this->random_generator.GetRandom() % card_count;
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

namespace std {
	template <> struct hash<GameEngine::Deck> {
		typedef GameEngine::Deck argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			for (auto card: s.cards) {
				GameEngine::hash_combine(result, card);
			}

			return result;
		}
	};
}

#endif
