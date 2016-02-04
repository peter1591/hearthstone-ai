#include <fstream>
#include "json/reader.h"
#include "game-engine/card-database.h"

namespace GameEngine {

	static Card::Rarity GetRarity(Json::Value const& json_card)
	{
		if (!json_card.isMember("rarity")) return Card::RARITY_UNKNOWN;

		std::string rarity = json_card["rarity"].asString();

		if (rarity == "FREE") return Card::RARITY_FREE;
		if (rarity == "COMMON") return Card::RARITY_COMMON;
		if (rarity == "RARE") return Card::RARITY_RARE;
		if (rarity == "EPIC") return Card::RARITY_EPIC;
		if (rarity == "LEGENDARY") return Card::RARITY_LEGENDARY;

		throw std::runtime_error("parse error");
	}

	static Card::MinionRace GetMinionRace(Json::Value const& json_card)
	{
		if (!json_card.isMember("race")) return Card::RACE_NORMAL;

		std::string race = json_card["race"].asString();
		if (race == "BEAST") return Card::RACE_BEAST;
		if (race == "DEMON") return Card::RACE_DEMON;
		if (race == "DRAGON") return Card::RACE_DRAGON;
		if (race == "MECHANICAL") return Card::RACE_MECH;
		if (race == "MURLOC") return Card::RACE_MURLOC;
		if (race == "PIRATE") return Card::RACE_PIRATE;
		if (race == "TOTEM") return Card::RACE_TOTEM;

		throw std::runtime_error("unknown race");
	}

	static void ProcessMinionCardMechanics(std::string const& origin_id, Json::Value const& json_card, Card & new_card)
	{
		if (!json_card.isMember("mechanics")) return;

		Json::Value json_mechanics = json_card["mechanics"];

		if (json_mechanics.isArray() == false) return;

		new_card.data.minion.taunt = false;
		new_card.data.minion.charge = false;
		new_card.data.minion.shield = false;
		new_card.data.minion.stealth = false;

		for (auto const& json_mechanic: json_mechanics)
		{
			std::string mechanic = json_mechanic.asString();
			if (mechanic == "TAUNT") {
				new_card.data.minion.taunt = true;
			}
			else if (mechanic == "CHARGE") {
				new_card.data.minion.charge = true;
			}
			else if (mechanic == "DIVINE_SHIELD") {
				new_card.data.minion.shield = true;
			}
			else if (mechanic == "STEALTH") {
				new_card.data.minion.stealth = true;
			}
			else if (mechanic == "FORGETFUL") { // TODO
			}
			else if (mechanic == "FREEZE") { // TODO
			}
			else if (mechanic == "POISONOUS") { // TODO
			}
			else if (mechanic == "WINDFURY") { // TODO
			}
			else if (mechanic == "OVERLOAD") { // TODO
			}
			else if (mechanic == "AURA" ||
				mechanic == "DEATHRATTLE" ||
				mechanic == "INSPIRE" ||
				mechanic == "BATTLECRY" ||
				mechanic == "SPELLPOWER" ||
				mechanic == "COMBO" ||
				mechanic == "ENRAGED" ||
				mechanic == "ADJACENT_BUFF" ||
				mechanic == "InvisibleDeathrattle" ||
				mechanic == "ImmuneToSpellpower") {
				// write hard-coded
			}
			else {
				throw std::runtime_error("unknown error");
			}
		}
	}

	CardDatabase::CardDatabase()
	{
		this->final_cards = nullptr;
		this->Clear();
	}

	CardDatabase & CardDatabase::GetInstance()
	{
		static CardDatabase instance;
		return instance;
	}

	bool CardDatabase::ReadFromJsonFile(std::string const & filepath)
	{
		Json::Reader reader;
		Json::Value cards_json;

		std::ifstream cards_file(filepath);

		if (reader.parse(cards_file, cards_json, false) == false) return false;

		return this->ReadFromJson(cards_json);
	}

	bool CardDatabase::ReadFromJson(Json::Value const & cards_json)
	{
		this->Clear();

		if (cards_json.isArray() == false) return false;

		for (auto const& card_json : cards_json)
		{
			this->AddCard(card_json);
		}

		int max_card_id = this->next_card_id;
		if (this->final_cards != nullptr)
		{
			delete[] this->final_cards;
		}
		this->final_cards = new GameEngine::Card[max_card_id];
		this->final_cards_size = max_card_id;
		for (auto const& card_info : this->cards)
		{
			this->final_cards[card_info.first] = card_info.second;
		}

		return true;
	}

	bool CardDatabase::AddCard(Json::Value const & card)
	{
		std::string origin_id = card["id"].asString();

		std::string type = card["type"].asString();

		Card new_card;

		if (type == "MINION") {
			this->AddMinionCard(origin_id, card, new_card);
		}
		else if (type == "SPELL") {
			this->AddSpellCard(origin_id, card, new_card);
		}
		else {
			// ignored
			return false;
		}

		// fix error data
		if (origin_id == "GVG_065")
		{
			new_card.data.minion.forgetful = true;
		}

		new_card.id = this->GetAvailableCardId();

		this->AddCard(new_card, origin_id);

		return true;
	}

	void CardDatabase::AddMinionCard(std::string  const& origin_id, Json::Value const & json_card, Card & new_card)
	{
		new_card.rarity = GetRarity(json_card);
		new_card.type = Card::TYPE_MINION;
		new_card.cost = json_card["cost"].asInt();
		new_card.data.minion.Clear();
		new_card.data.minion.attack = json_card["attack"].asInt();
		new_card.data.minion.hp = json_card["health"].asInt();
		new_card.data.minion.race = GetMinionRace(json_card);

		ProcessMinionCardMechanics(origin_id, json_card, new_card);
	}

	void CardDatabase::AddSpellCard(std::string  const& origin_id, Json::Value const & json_card, Card & new_card)
	{
		new_card.rarity = GetRarity(json_card);
		new_card.type = Card::TYPE_SPELL;
		new_card.cost = json_card["cost"].asInt();
		new_card.id = this->GetAvailableCardId();
		new_card.data.spell.Clear();
	}

	void CardDatabase::AddCard(Card const & card, std::string const& origin_id)
	{
		this->cards[card.id] = card;

		if (this->origin_id_map.find(origin_id) != this->origin_id_map.end())
		{
			throw std::runtime_error("origin id collision.");
		}

		this->origin_id_map[origin_id] = card.id;
	}

	void CardDatabase::Clear()
	{
		this->cards.clear();
		this->next_card_id = 1;
	}

	int CardDatabase::GetAvailableCardId()
	{
		int id = this->next_card_id;
		++this->next_card_id;
		return id;
	}

	Card CardDatabase::GetCard(int card_id) const
	{
		if (card_id < 0 || card_id >= this->final_cards_size) throw std::runtime_error("out of range");
		return this->final_cards[card_id];
	}

	std::unordered_map<std::string, int> const & CardDatabase::GetOriginalIdMap() const
	{
		return this->origin_id_map;
	}

	int CardDatabase::GetCardIdFromOriginalId(std::string const & origin_id) const
	{
		auto it = this->origin_id_map.find(origin_id);
		if (it == this->origin_id_map.end()) return -1;
		return it->second;
	}

} // namespace