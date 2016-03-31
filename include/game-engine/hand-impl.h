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

	inline void Hand::AddDeterminedCard(Card const & card)
	{
		HandCard hand_card;
		hand_card.type = HandCard::TYPE_DETERMINED;
		hand_card.card = card;
		this->cards.push_back(hand_card);
	}

	inline bool Hand::HasCardToDraw() const
	{
		return this->deck_cards.empty();
	}

	inline void Hand::DrawOneCardToHand()
	{
		HandCard hand_card;
		hand_card.type = HandCard::TYPE_DETERMINED; // TODO: add a new type TYPE_DRAW_FROM_HIDDEN_CARDS
		hand_card.card = this->DrawOneCardFromDeck();
		this->cards.push_back(hand_card);
	}

	inline Card Hand::DrawOneCardAndDiscard()
	{
		return this->DrawOneCardFromDeck();
	}

	inline Card Hand::DrawOneCardFromDeck()
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

	inline Card const & Hand::GetCard(Locator idx) const
	{
		return (this->cards.begin() + idx)->card;
	}

	inline void Hand::RemoveCard(Locator idx)
	{
		std::vector<HandCard>::iterator it = this->cards.begin() + idx;
		this->cards.erase(it);
	}

	inline size_t Hand::GetCount() const
	{
		return this->cards.size();
	}

	inline void Hand::AddCardToDeck(Card const & card)
	{
		this->deck_cards.push_back(card);
	}

	inline bool Hand::operator==(const Hand &rhs) const
	{
		// special logic to compare different kinds of hand cards
		if (this->cards.size() != rhs.cards.size()) return false;
		for (size_t i = 0; i < this->cards.size(); ++i) {
			auto const& lhs_card = this->cards[i];
			auto const& rhs_card = rhs.cards[i];

			if (lhs_card.type != rhs_card.type) return false;

			switch (lhs_card.type) {
			case HandCard::TYPE_DETERMINED:
				if (lhs_card.card != rhs_card.card) return false;
				break;

			default:
				throw std::runtime_error("invalid hand card type");
			}
		}

		if (this->deck_cards != rhs.deck_cards) return false;

		return true;
	}

	inline bool Hand::operator!=(Hand const& rhs) const
	{
		return !(*this == rhs);
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
			result += card.card.GetDebugString() + " ";
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
				GameEngine::hash_combine(result, card.type);

				switch (card.type)
				{
				case GameEngine::HandCard::TYPE_DETERMINED:
					GameEngine::hash_combine(result, card.card);
					break;

				default:
					throw std::runtime_error("invalid hand card type");
				}
			}

			for (auto deck_card : s.deck_cards) {
				GameEngine::hash_combine(result, deck_card);
			}

			return result;
		}
	};
}