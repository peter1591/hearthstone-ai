#pragma once

#include <unordered_map>
#include <json/value.h>
#include "card.h"

namespace GameEngine {

	class CardDatabase
	{
	public:
		CardDatabase();

		bool ReadFromJsonFile(std::string const& filepath);
		bool ReadFromJson(Json::Value const& json);

		Card GetCard(int card_id) const;

		std::unordered_map<std::string, int> const& GetOriginalIdMap() const;
		int GetCardIdFromOriginalId(std::string const& origin_id) const;

	private:
		void AddCard(Card const & card, std::string const & origin_id);
		bool AddCard(Json::Value const& card);
		bool AddMinionCard(Json::Value const& card);
		bool AddSpellCard(Json::Value const& card);

		void Clear();

		int GetAvailableCardId();

	private:
		std::unordered_map<int, Card> cards;
		std::unordered_map<std::string, int> origin_id_map;
		int next_card_id;
	};

} // namespace GameEngine