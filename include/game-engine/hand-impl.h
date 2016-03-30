#pragma once

#include "random-generator.h"
#include "hand.h"

namespace GameEngine {
	inline Hand::Hand(RandomGenerator & random_generator) :
		random_generator(random_generator)
	{
		this->cards.reserve(10);
		this->deck_cards.reserve(36);
	}

	inline Hand::Hand(RandomGenerator & random_generator, Hand const& rhs) :
		random_generator(random_generator), cards(rhs.cards), deck_cards(rhs.deck_cards)
	{
	}

	inline Hand::Hand(RandomGenerator & random_generator, Hand && rhs) :
		random_generator(random_generator),
		cards(std::move(rhs.cards)), deck_cards(std::move(rhs.deck_cards))
	{
	}

	inline Hand & Hand::operator=(Hand && rhs)
	{
		this->cards = std::move(rhs.cards);
		this->deck_cards = std::move(rhs.deck_cards);
		return *this;
	}

	inline void Hand::Initialize_AddHandCard(Card const & card)
	{
		this->cards.push_back(card);
	}

	inline void Hand::DrawOneCardToHand()
	{
		Card draw_card = this->DrawFromDeck();

		this->cards.push_back(draw_card);
	}

	inline Card Hand::DrawOneCardAndDiscard()
	{
		return this->DrawFromDeck();
	}

	inline void Hand::RemoveCard(Locator idx)
	{
		std::vector<Card>::iterator it = this->cards.begin() + idx;
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

			for (auto card : s.cards) {
				GameEngine::hash_combine(result, card);
			}

			for (auto deck_card : s.deck_cards) {
				GameEngine::hash_combine(result, deck_card);
			}

			return result;
		}
	};
}