#pragma once

#include <assert.h>
#include <string>
#include <fstream>
#include <unordered_map>
#include "json/json.h"

#include "State/Types.h"
#include "State/Cards/CardData.h"

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

		state::Cards::CardData const& Get(int id)
		{
			assert(id >= 0);
			assert(id < final_cards_size_);
			return final_cards_[id];
		}

	private:
		Database() { }

		bool ReadFromJson(Json::Value const & cards_json)
		{
			if (cards_json.isArray() == false) return false;

			std::vector<state::Cards::CardData> cards;

			// Reserve id = 0
			cards.push_back(state::Cards::CardData());

			origin_id_map_.clear();
			for (auto const& card_json : cards_json) {
				this->AddCard(card_json, cards);
			}

			if (final_cards_) { delete[] final_cards_; }

			final_cards_size_ = (int)cards.size();
			final_cards_ = new state::Cards::CardData[final_cards_size_];

			// Copy to raw array to support lock-free access
			for (int i = 0; i < cards.size(); ++i) {
				final_cards_[i] = cards[i];
			}

			return true;
		}

		void AddCard(Json::Value const& json, std::vector<state::Cards::CardData> & cards)
		{
			const std::string origin_id = json["id"].asString();
			const std::string type = json["type"].asString();

			if (origin_id_map_.find(origin_id) != origin_id_map_.end()) {
				throw std::exception("Card ID string collision.");
			}

			state::Cards::CardData new_card;
			new_card.card_id = (int)cards.size();

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

			origin_id_map_[origin_id] = new_card.card_id;
			cards.push_back(new_card);
		}

		void ParseMinionCard(Json::Value const& json, state::Cards::CardData & card)
		{
			card.card_type = state::kCardTypeMinion;
			card.enchantable_states.cost = json["cost"].asInt();
			card.enchantable_states.attack = json["attack"].asInt();
			card.enchantable_states.max_hp = json["health"].asInt();
		}

		void ParseSpellCard(Json::Value const& json, state::Cards::CardData & card)
		{
			card.card_type = state::kCardTypeSpell;
			// TODO
		}

		void ParseWeaponCard(Json::Value const& json, state::Cards::CardData & card)
		{
			card.card_type = state::kCardTypeWeapon;
			// TODO
		}

		void ParseHeroPowerCard(Json::Value const& json, state::Cards::CardData & card)
		{
			card.card_type = state::kCardTypeHeroPower;
			// TODO
		}

		void ParseMechanics(Json::Value const& json, state::Cards::CardData & card)
		{
			card.enchantable_states.mechanics.taunt = false;
			card.enchantable_states.mechanics.charge = false;
			card.enchantable_states.mechanics.shield = false;
			card.enchantable_states.mechanics.stealth = false;
			card.enchantable_states.mechanics.forgetful = false;
			card.enchantable_states.mechanics.freeze = false;
			card.enchantable_states.mechanics.windfury = false;
			card.enchantable_states.mechanics.poisonous = false;

			if (!json.isMember("mechanics")) return;

			Json::Value json_mechanics = json["mechanics"];

			if (json_mechanics.isArray() == false) return;

			for (auto const& json_mechanic : json_mechanics)
			{
				std::string mechanic = json_mechanic.asString();
				if (mechanic == "TAUNT") {
					card.enchantable_states.mechanics.taunt = true;
				}
				else if (mechanic == "CHARGE") {
					card.enchantable_states.mechanics.charge = true;
				}
				else if (mechanic == "DIVINE_SHIELD") {
					card.enchantable_states.mechanics.shield = true;
				}
				else if (mechanic == "STEALTH") {
					card.enchantable_states.mechanics.stealth = true;
				}
				else if (mechanic == "FORGETFUL") {
					card.enchantable_states.mechanics.forgetful = true;
				}
				else if (mechanic == "FREEZE") {
					card.enchantable_states.mechanics.freeze = true;
				}
				else if (mechanic == "WINDFURY") {
					card.enchantable_states.mechanics.windfury = true;
				}
				else if (mechanic == "POISONOUS") {
					card.enchantable_states.mechanics.poisonous = true;
				}
				else if (mechanic == "SPELLPOWER") { // TODO
				}
				else if (mechanic == "OVERLOAD") { // TODO
				}
				else if (mechanic == "AURA" ||
					mechanic == "DEATHRATTLE" ||
					mechanic == "INSPIRE" ||
					mechanic == "BATTLECRY" ||
					mechanic == "COMBO" ||
					mechanic == "ENRAGED" ||
					mechanic == "ADJACENT_BUFF" ||
					mechanic == "InvisibleDeathrattle" ||
					mechanic == "ImmuneToSpellpower") {
					// write hard-coded
				}
				else {
					throw std::runtime_error("unsupported mechanic");
				}
			}
		}

	private:
		state::Cards::CardData * final_cards_; // Raw array to support lock-free access
		int final_cards_size_;

		std::unordered_map<std::string, int> origin_id_map_;
	};
}