#pragma once

#include <vector>

#include <json/json.h>

#include "Cards/id-map.h"

namespace ui
{
	namespace board
	{
		class OrderedEntities
		{
		public:
			void SetEntityId(size_t pos, int entity_id) {
				if (pos >= items_.size()) {
					items_.resize(pos + 1);
				}
				items_[pos] = entity_id;
			}

			template <class Functor>
			void ForEach(Functor && functor) const {
				for (int entity_id : items_) {
					functor(entity_id);
				}
			}

		private:
			std::vector<int> items_;
		};

		struct Resource
		{
			int this_turn;
			int total;
			int overload;
			int overload_next_turn;

			void Parse(Json::Value const& json) {
				this_turn = json["this_turn"].asInt();
				total = json["total"].asInt();
				overload = json["overload"].asInt();
				overload_next_turn = json["overload_next_turn"].asInt();
			}
		};

		struct CharacterStatus
		{
			bool charge;
			bool taunt;
			bool divine_shield;
			bool stealth;
			bool freeze;
			bool frozon;
			bool poisonous;
			bool windfury;

			void Parse(Json::Value const& json) {
				charge = json["charge"].asBool();
				taunt = json["taunt"].asBool();
				divine_shield = json["divine_shield"].asBool();
				stealth = json["stealth"].asBool();
				freeze = json["freeze"].asBool();
				frozon = json["frozon"].asBool();
				poisonous = json["poisonous"].asBool();
				windfury = json["windfury"].asBool();
			}
		};

		struct Utils {
			static Cards::CardId GetCardId(std::string const& card_id)
			{
				auto const& container = Cards::Database::GetInstance().GetIdMap();
				auto it = container.find(card_id);
				if (it == container.end()) {
					return Cards::kInvalidCardId;
				}
				return (Cards::CardId)it->second;
			}
		};

		struct Hero
		{
			Cards::CardId card_id;
			int max_hp;
			int damage;
			int armor;
			int attack;
			int attack_this_turn;
			CharacterStatus status;

			void Parse(Json::Value const& json) {
				card_id = Utils::GetCardId(json["card_id"].asString());
				max_hp = json["max_hp"].asInt();
				damage = json["damage"].asInt();
				armor = json["armor"].asInt();
				attack = json["attack"].asInt();
				attack_this_turn = json["attack_this_turn"].asInt();
				status.Parse(json["status"]);
			}
		};

		struct HeroPower {
			Cards::CardId card_id;
			bool used;

			void Parse(Json::Value const& json) {
				card_id = Utils::GetCardId(json["card_id"].asString());
				used = json["used"].asBool();
			}
		};

		struct Minion {
			Cards::CardId card_id;
			int max_hp;
			int damage;
			int attack;
			int attack_this_turn;
			CharacterStatus status;
			bool silenced;
			int spellpower;
			bool summoned_this_turn;

			void Parse(Json::Value const& json) {
				card_id = Utils::GetCardId(json["card_id"].asString());
				max_hp = json["max_hp"].asInt();
				damage = json["damage"].asInt();
				attack = json["attack"].asInt();
				attack_this_turn = json["attack_this_turn"].asInt();
				status.Parse(json["status"]);
				silenced = json["silenced"].asBool();
				spellpower = json["spellpower"].asInt();
				summoned_this_turn = json["summoned_this_turn"].asBool();
			}
		};

		struct Minions {
			std::vector<Minion> minions;

			void Parse(Json::Value const& json) {
				for (Json::ArrayIndex idx = 0; idx < json.size(); ++idx)
				{
					minions.emplace_back();
					minions.back().Parse(json[idx]);
				}
			}
		};

		struct Player
		{
			/*
			int player_entity_id;
			int hero_entity_id;
			int weapon_entity_id;
			int hero_power_entity_id;

			OrderedEntities hand;
			*/

			Hero hero;
			HeroPower hero_power;
			Minions minions;
			int fatigue;
			Resource resource;
			
			std::vector<int> hand;
			std::vector<int> deck;

			void Parse(Json::Value const& json) {
				hero.Parse(json["hero"]);
				hero_power.Parse(json["hero"]["hero_power"]);
				minions.Parse(json["minions"]);
				fatigue = json["fatigue"].asInt();
				resource.Parse(json["crystal"]);

				Json::Value const& deck_entities = json["deck"]["entities"];
				deck.clear();
				for (Json::ArrayIndex idx = 0; idx < deck_entities.size(); ++idx) {
					deck.push_back(deck_entities[idx].asInt());
				}

				Json::Value const& hand_entities = json["hand"]["entities"];
				hand.clear();
				for (Json::ArrayIndex idx = 0; idx < hand_entities.size(); ++idx) {
					hand.push_back(hand_entities[idx].asInt());
				}
			}
		};
	}
}