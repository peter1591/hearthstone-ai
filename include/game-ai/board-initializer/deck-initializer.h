#pragma once

#include <set>
#include "json/json.h"
#include "game-engine/board.h"
#include "game-engine/card-id-map.h"
#include "game-engine/card-database.h"

class DeckInitializer
{
public:
	void SetDeck_BasicPracticeMage()
	{
		this->origin_deck.insert(CARD_ID_CS2_025); // Arcane Explosion
		this->origin_deck.insert(CARD_ID_CS2_025); // Arcane Explosion
		this->origin_deck.insert(CARD_ID_CS2_023); // Arcane Intellct
		this->origin_deck.insert(CARD_ID_CS2_023); // Arcane Intellct
		this->origin_deck.insert(CARD_ID_EX1_277); // Arcane Missiles
		this->origin_deck.insert(CARD_ID_EX1_277); // Arcane Missiles
		this->origin_deck.insert(CARD_ID_CS2_172); // 232
		this->origin_deck.insert(CARD_ID_CS2_172); // 232
		this->origin_deck.insert(CARD_ID_CS2_200); // 667
		this->origin_deck.insert(CARD_ID_CS2_200); // 667
		this->origin_deck.insert(CARD_ID_CS2_029); // Fireball
		this->origin_deck.insert(CARD_ID_CS2_029); // Fireball
		this->origin_deck.insert(CARD_ID_CS2_168); // Murloc Raider 121
		this->origin_deck.insert(CARD_ID_CS2_168); // Murloc Raider 121
		this->origin_deck.insert(CARD_ID_EX1_593); // Nightblade
		this->origin_deck.insert(CARD_ID_EX1_593); // Nightblade
		this->origin_deck.insert(CARD_ID_EX1_015); // Novice Enginner
		this->origin_deck.insert(CARD_ID_EX1_015); // Novice Enginner
		this->origin_deck.insert(CARD_ID_CS2_119); // 427
		this->origin_deck.insert(CARD_ID_CS2_119); // 427
		this->origin_deck.insert(CARD_ID_CS2_022); // Polymorph
		this->origin_deck.insert(CARD_ID_CS2_022); // Polymorph
		this->origin_deck.insert(CARD_ID_CS2_122); // Raid Leader
		this->origin_deck.insert(CARD_ID_CS2_122); // Raid Leader
		this->origin_deck.insert(CARD_ID_CS2_120); // River Crocolisk 223
		this->origin_deck.insert(CARD_ID_CS2_120); // River Crocolisk 223
		this->origin_deck.insert(CARD_ID_CS2_179); // 435 Taunt
		this->origin_deck.insert(CARD_ID_CS2_179); // 435 Taunt
		this->origin_deck.insert(CARD_ID_CS2_124); // Wolf Rider 331 Charge
		this->origin_deck.insert(CARD_ID_CS2_124); // Wolf Rider 331 Charge
	}

	void SetDeck_BasicPracticeWarlock()
	{
		this->origin_deck.insert(CARD_ID_CS2_182); // Chillwind Yeti
		this->origin_deck.insert(CARD_ID_CS2_182); // Chillwind Yeti
		this->origin_deck.insert(CARD_ID_DS1_055); // Darkscale Healer
		this->origin_deck.insert(CARD_ID_DS1_055); // Darkscale Healer
		this->origin_deck.insert(CARD_ID_CS2_061); // Drain Life
		this->origin_deck.insert(CARD_ID_CS2_061); // Drain Life
		this->origin_deck.insert(CARD_ID_CS2_062); // Hellfire
		this->origin_deck.insert(CARD_ID_CS2_062); // Hellfire
		this->origin_deck.insert(CARD_ID_CS2_142); // Kobold Geomancer
		this->origin_deck.insert(CARD_ID_CS2_142); // Kobold Geomancer
		this->origin_deck.insert(CARD_ID_CS2_168); // Murloc Raider 121
		this->origin_deck.insert(CARD_ID_CS2_168); // Murloc Raider 121
		this->origin_deck.insert(CARD_ID_CS2_197); // Ogre Magi
		this->origin_deck.insert(CARD_ID_CS2_197); // Ogre Magi
		this->origin_deck.insert(CARD_ID_CS2_213); // Reckless Rocketeer 652 CHARGE
		this->origin_deck.insert(CARD_ID_CS2_213); // Reckless Rocketeer 652 CHARGE
		this->origin_deck.insert(CARD_ID_CS2_120); // River Crocolisk 223
		this->origin_deck.insert(CARD_ID_CS2_120); // River Crocolisk 223
		this->origin_deck.insert(CARD_ID_CS2_057); // Shadow Bolt
		this->origin_deck.insert(CARD_ID_CS2_057); // Shadow Bolt
		this->origin_deck.insert(CARD_ID_EX1_306); // Succubus
		this->origin_deck.insert(CARD_ID_EX1_306); // Succubus
		this->origin_deck.insert(CARD_ID_CS2_065); // Voidwalker
		this->origin_deck.insert(CARD_ID_CS2_065); // Voidwalker
		this->origin_deck.insert(CARD_ID_EX1_011); // Woodoo doctor
		this->origin_deck.insert(CARD_ID_EX1_011); // Woodoo doctor
		this->origin_deck.insert(CARD_ID_CS2_186); // War Golem
		this->origin_deck.insert(CARD_ID_CS2_186); // War Golem
		this->origin_deck.insert(CARD_ID_CS2_124); // Wolf Rider 331 Charge
		this->origin_deck.insert(CARD_ID_CS2_124); // Wolf Rider 331 Charge
	}

	void InitializeHand(Json::Value const& played_cards, Json::Value const& hand_cards, GameEngine::Hand & hand) const
	{
		std::multiset<int> deck(this->origin_deck);

		for (auto const& played_card : played_cards)
		{
			int card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(played_card.asString());
			if (card_id < 0) {
				continue;
			}
			auto it = deck.find(card_id);
			if (it == deck.end()) {
				continue;
			}
			deck.erase(it);
		}

		int hand_cards_draw_from_deck = 0;
		for (auto const& hand_card : hand_cards)
		{
			int card_id = GameEngine::CardDatabase::GetInstance().GetCardIdFromOriginalId(hand_card.asString());
			if (card_id < 0) {
				hand_cards_draw_from_deck++;
				continue;
			}
			hand.AddDeterminedCard(GameEngine::CardDatabase::GetInstance().GetCard(card_id));

			auto it = deck.find(card_id);
			if (it == deck.end()) {
				continue;
			}
			deck.erase(it);
		}

		for (auto const& deck_card : deck) {
			hand.AddCardToDeck(GameEngine::CardDatabase::GetInstance().GetCard(deck_card));
		}

		for (int i = 0; i < hand_cards_draw_from_deck; ++i) {
			hand.DrawOneCardToHand();
		}
	}

private:
	std::multiset<int> origin_deck;
};