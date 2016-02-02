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
	Hand();

	void AddCard(const Card &card);

	const std::vector<Card> &GetCards() const { return this->cards; }
	Card const& GetCard(Locator idx) const { return *(this->cards.begin() + idx); }

	void RemoveCard(Locator idx);

	size_t GetCount() const { return this->cards.size(); }
	int GetCountByCardType(Card::Type t) const;

	bool operator==(const Hand &rhs) const;
	bool operator!=(const Hand &rhs) const { return !(*this == rhs); }

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

inline void Hand::RemoveCard(Locator idx)
{
	std::vector<Card>::iterator it = this->cards.begin() + idx;
	this->count_by_type[it->type]--;
	this->cards.erase(it);
}

inline bool Hand::operator==(const Hand &rhs) const
{
	if (this->cards != rhs.cards) return false;

	// no need to check count_by_type

	return true;
}

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::Hand> {
		typedef GameEngine::Hand argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			for (auto card: s.cards) {
				GameEngine::hash_combine(result, hash<decltype(card)>()(card));
			}

			return result;
		}
	};

}

#endif
