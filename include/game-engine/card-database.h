#pragma once

#include <unordered_map>
#include <json/value.h>
#include "card.h"

namespace GameEngine {

	class CardDatabase
	{
	public:
		static CardDatabase & GetInstance();

		bool ReadFromJsonFile(std::string const& filepath);
		bool ReadFromJson(Json::Value const& json);

		// thread safe
		Card GetCard(int card_id) const;

		int GetAvailableCardId();
		std::unordered_map<std::string, int> const& GetOriginalIdMap() const;
		int GetCardIdFromOriginalId(std::string const& origin_id) const;

	private:
		CardDatabase();

		void AddCard(Card const & card, std::string const & origin_id);
		bool AddCard(Json::Value const& card);
		bool AddMinionCard(Json::Value const& card, Card & new_card);
		bool AddSpellCard(Json::Value const& card, Card & new_card);

		void Clear();

	private:
		Card * final_cards; // array of cards; good for thread-safe usage
		std::unordered_map<int, Card> cards;
		std::unordered_map<std::string, int> origin_id_map;
		int next_card_id;
	};

} // namespace GameEngine