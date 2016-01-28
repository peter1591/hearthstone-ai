#ifndef GAME_ENGINE_OPPONENT_CARDS_H
#define GAME_ENGINE_OPPONENT_CARDS_H

#include <functional>
#include <sstream>
#include <string>
#include <vector>
#include "player-stat.h"
#include "card.h"

namespace GameEngine {

class OpponentCards
{
	friend std::hash<OpponentCards>;

	public:
		OpponentCards() : deck_card_count(0), hand_card_count(0) {}

		void Set(int deck_size) { this->deck_card_count = deck_size; }
		int GetHandCount() const { return this->hand_card_count; }
		int GetDeckCount() const { return this->deck_card_count; }

		void GetPossiblePlayableMinions(const PlayerStat &opponent_stat, std::vector<Card> &playable_cards) const
		{
			if (opponent_stat.crystal.GetCurrent() < 3) return;

			// TODO
			Card card;
			card.type = Card::TYPE_MINION;
			card.id = 223;
			card.cost = 2;
			card.data.minion.attack = 2;
			card.data.minion.hp = 3;

			playable_cards.push_back(card);
		}

		void AddCardToDeck(const Card &) {
			++this->deck_card_count;
			// TODO: record the added card
		}

		void PlayCardFromHand(const Card &) {
			--this->hand_card_count;
		}

		void DrawFromDeckToHand() {
			--this->deck_card_count;
			++this->hand_card_count;
		}

		// TODO: we can guess the opponent deck type

		std::string GetDebugString() const;

		bool operator==(const OpponentCards &rhs) const {
			if (this->deck_card_count != rhs.deck_card_count) return false;
			if (this->hand_card_count != rhs.hand_card_count) return false;
			return true;
		}

		bool operator!=(const OpponentCards &rhs) const {
			return !(*this == rhs);
		}

	private:
		int deck_card_count;
		int hand_card_count;
};

inline std::string OpponentCards::GetDebugString() const
{
	std::ostringstream oss;
	oss << "Hand: " << this->hand_card_count
		<< ", Deck: " << this->deck_card_count;
	return oss.str();
}

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::OpponentCards> {
		typedef GameEngine::OpponentCards argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, hash<decltype(s.deck_card_count)>()(s.deck_card_count));
			GameEngine::hash_combine(result, hash<decltype(s.hand_card_count)>()(s.hand_card_count));

			return result;
		}
	};

}

#endif
