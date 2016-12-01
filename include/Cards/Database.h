#pragma once

#include <string>
#include <fstream>
#include <unordered_map>
#include "json/json.h"

#include "State/Types.h"
#include "State/Cards/RawCard.h"

namespace Cards
{
	class Database
	{
	public:
		static Database & GetInstance()
		{
			static Database instance;
			return instance;
		}

	public:
		bool LoadJsonFile(std::string const& path)
		{
			Json::Reader reader;
			Json::Value cards_json;

			std::ifstream cards_file(path);

			if (reader.parse(cards_file, cards_json, false) == false) return false;

			return this->ReadFromJson(cards_json);
		}

		std::unordered_map<std::string, int> GetIdMap() const { return origin_id_map_; }

	private:
		Database() { }

		bool ReadFromJson(Json::Value const & cards_json)
		{
			if (cards_json.isArray() == false) return false;

			std::vector<state::Cards::RawCard> cards;

			// Reserve id = 0
			cards.push_back(state::Cards::RawCard());

			origin_id_map_.clear();
			for (auto const& card_json : cards_json) {
				this->AddCard(card_json, cards);
			}

			if (final_cards_) { delete[] final_cards_; }

			final_cards_size_ = (int)cards.size();
			final_cards_ = new state::Cards::RawCard[final_cards_size_];

			// Copy to raw array to support lock-free access
			for (int i = 0; i < cards.size(); ++i) {
				final_cards_[i] = cards[i];
			}

			return true;
		}

		void AddCard(Json::Value const& json, std::vector<state::Cards::RawCard> & cards)
		{
			const std::string origin_id = json["id"].asString();
			const std::string type = json["type"].asString();

			if (origin_id_map_.find(origin_id) != origin_id_map_.end()) {
				throw std::exception("Card ID string collision.");
			}

			state::Cards::RawCard new_card;

			if (type == "MINION") {
				this->ParseMinionCard(json, new_card);
			}
			else if (type == "SPELL") {
				this->ParseSpellCard(json, new_card);
			}
			else if (type == "WEAPON") {
				this->ParseWeaponCard(json, new_card);
			}
			else if (type == "HERO_POWER") {
				this->ParseHeroPowerCard(json, new_card);
			}
			else {
				return; // ignored
			}

			int id = (int)cards.size();
			origin_id_map_[origin_id] = id;
			cards.push_back(new_card);
		}

		void ParseMinionCard(Json::Value const& json, state::Cards::RawCard & card)
		{
			card.card_type = state::kCardTypeMinion;
			card.enchanted_states.cost = json["cost"].asInt();
			card.enchanted_states.attack = json["attack"].asInt();
			card.enchanted_states.max_hp = json["health"].asInt();
		}

		void ParseSpellCard(Json::Value const& json, state::Cards::RawCard & card)
		{
			card.card_type = state::kCardTypeSpell;
			// TODO
		}

		void ParseWeaponCard(Json::Value const& json, state::Cards::RawCard & card)
		{
			card.card_type = state::kCardTypeWeapon;
			// TODO
		}

		void ParseHeroPowerCard(Json::Value const& json, state::Cards::RawCard & card)
		{
			card.card_type = state::kCardTypeHeroPower;
			// TODO
		}

	private:
		state::Cards::RawCard * final_cards_; // Raw array to support lock-free access
		int final_cards_size_;

		std::unordered_map<std::string, int> origin_id_map_;
	};
}