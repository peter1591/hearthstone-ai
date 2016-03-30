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

	class CardsBitmap
	{
	public:
		bool None() const { return this->bitmap.none(); }

		void SetOneCard(int idx) { this->bitmap.set(idx); }

		int GetOneCard() const {
			for (int i = 0; i < max_cards; ++i) {
				if (this->bitmap[i]) return i;
			}
			throw std::runtime_error("no card available");
		}

		void ClearOneCard(int idx) { this->bitmap.set(idx, false); }

		bool operator==(CardsBitmap const& rhs) const {
			return this->bitmap == rhs.bitmap;
		}

		bool operator!=(CardsBitmap const& rhs) const {
			return !(*this == rhs);
		}
		
	private:
		static constexpr int max_cards = 10;
		std::bitset<max_cards> bitmap;
	};

public:
	Hand(RandomGenerator & random_generator);
	Hand(RandomGenerator & random_generator, Hand const& rhs);
	Hand & operator=(Hand const& rhs) = delete;
	Hand(RandomGenerator & random_generator, Hand && rhs);
	Hand & operator=(Hand && rhs);

	void AddCard(const Card &card);
	Card const& GetCard(Locator idx) const { return *(this->cards.begin() + idx); }
	void RemoveCard(Locator idx);
	size_t GetCount() const { return this->cards.size(); }
	int GetCountByCardType(Card::Type t) const;

	void AddCardToDeck(Card const& card);
	bool IsDeckEmpty() const;
	Card DrawFromDeck();

	bool operator==(const Hand &rhs) const;
	bool operator!=(const Hand &rhs) const { return !(*this == rhs); }

	std::string GetDebugString() const;

private:
	RandomGenerator & random_generator;

	std::vector<Card> cards;
	int count_by_type[Card::TYPE_MAX];

	std::vector<Card> deck_cards;
};

inline Hand::Hand(RandomGenerator & random_generator) :
	random_generator(random_generator)
{
	this->cards.reserve(10);
	for (int i=0; i<Card::TYPE_MAX; ++i) {
		this->count_by_type[i] = 0;
	}

	this->deck_cards.reserve(36);
}

inline Hand::Hand(RandomGenerator & random_generator, Hand const& rhs) :
	random_generator(random_generator), cards(rhs.cards), deck_cards(rhs.deck_cards)
{
	for (int i = 0; i<Card::TYPE_MAX; ++i) {
		this->count_by_type[i] = 0;
	}
}

inline Hand::Hand(RandomGenerator & random_generator, Hand && rhs) :
	random_generator(random_generator),
	cards(std::move(rhs.cards)), deck_cards(std::move(rhs.deck_cards))
{
	for (int i = 0; i<Card::TYPE_MAX; ++i) {
		this->count_by_type[i] = 0;
	}
}

inline Hand & Hand::operator=(Hand && rhs)
{
	this->cards = std::move(rhs.cards);
	this->deck_cards = std::move(rhs.deck_cards);
	for (int i = 0; i<Card::TYPE_MAX; ++i) {
		this->count_by_type[i] = 0;
	}
	return *this;
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

inline void Hand::RemoveCard(Locator idx)
{
	std::vector<Card>::iterator it = this->cards.begin() + idx;
	this->count_by_type[it->type]--;
	this->cards.erase(it);
}

inline void Hand::AddCardToDeck(Card const & card)
{
	this->deck_cards.push_back(card);
}

inline bool Hand::IsDeckEmpty() const
{
	return this->deck_cards.empty();
}

inline Card Hand::DrawFromDeck()
{
	Card ret;
	size_t deck_count = this->deck_cards.size();

	if (UNLIKELY(deck_count == 0)) {
		ret.MarkInvalid();
		return ret;
	}

	const int rand_idx = this->random_generator.GetRandom((int)deck_count);

	ret = this->deck_cards[rand_idx];

	this->deck_cards[rand_idx] = this->deck_cards.back();
	this->deck_cards.pop_back();

	return ret;
}

inline bool Hand::operator==(const Hand &rhs) const
{
	if (this->cards != rhs.cards) return false;
	if (this->deck_cards != rhs.deck_cards) return false;

	// no need to check count_by_type

	return true;
}

inline std::string Hand::GetDebugString() const
{
	std::string result;

	result += "Deck: ";
	for (const auto &deck_card : this->deck_cards) {
		result += deck_card.GetDebugString() + " ";
	}
	result += "\n";

	result += "Hand: ";
	for (const auto &card : this->cards) {
		result += card.GetDebugString() + " ";
	}

	return result;
}

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::Hand> {
		typedef GameEngine::Hand argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			for (auto card: s.cards) {
				GameEngine::hash_combine(result, card);
			}

			for (auto deck_card : s.deck_cards) {
				GameEngine::hash_combine(result, deck_card);
			}

			return result;
		}
	};

}

#endif
