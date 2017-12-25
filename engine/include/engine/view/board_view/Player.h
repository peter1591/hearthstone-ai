#pragma once

#include <vector>

#include <json/json.h>

#include "Cards/id-map.h"
#include "Cards/Database.h"
#include "engine/view/board_view/CardInfo.h"
#include "engine/view/board_view/Controllers.h"

namespace engine
{
	namespace view
	{
		namespace board_view {
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
				int this_turn; // crystals earns temporary this turn
				int used;
				int current;
				int total;
				int overload;
				int overload_next_turn;

				void Parse(Json::Value const& json) {
					this_turn = json["this_turn"].asInt();
					used = json["used"].asInt();
					total = json["total"].asInt();
					overload = json["overload"].asInt();
					overload_next_turn = json["overload_next_turn"].asInt();
					current = total + this_turn - used;
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
				int attacks_this_turn;
				CharacterStatus status;

				void Parse(Json::Value const& json) {
					card_id = Utils::GetCardId(json["card_id"].asString());
					max_hp = json["max_hp"].asInt();
					damage = json["damage"].asInt();
					armor = json["armor"].asInt();
					attack = json["attack"].asInt();
					attacks_this_turn = json["attacks_this_turn"].asInt();
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
				int attacks_this_turn;
				CharacterStatus status;
				bool silenced;
				int spellpower;
				bool summoned_this_turn;
				bool exhausted;

				void Parse(Json::Value const& json) {
					card_id = Utils::GetCardId(json["card_id"].asString());
					max_hp = json["max_hp"].asInt();
					damage = json["damage"].asInt();
					attack = json["attack"].asInt();
					attacks_this_turn = json["attacks_this_turn"].asInt();
					status.Parse(json["status"]);
					silenced = json["silenced"].asBool();
					spellpower = json["spellpower"].asInt();
					summoned_this_turn = json["summoned_this_turn"].asBool();
					exhausted = json["exhausted"].asBool();

					// Patch summoned_this_turn flag
					// If a minion is summoned from hero power or spell card, this flag will not be set
					if (exhausted) {
						if (attacks_this_turn == 0) {
							summoned_this_turn = true;
						}
					}
				}
			};

			struct Minions {
				std::vector<Minion> minions;

				void Parse(Json::Value const& json) {
					minions.clear();
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

				std::vector<CardInfo> hand;
				std::vector<CardInfo> deck;

				void Parse(Json::Value const& json, Json::Value const& json_entities, Controllers & controllers) {
					hero.Parse(json["hero"]);
					hero_power.Parse(json["hero"]["hero_power"]);
					minions.Parse(json["minions"]);
					fatigue = json["fatigue"].asInt();
					resource.Parse(json["crystal"]);

					Json::Value const& deck_entities = json["deck"]["entities"];
					deck.clear();
					for (Json::ArrayIndex idx = 0; idx < deck_entities.size(); ++idx) {
						int entity_id = deck_entities[idx].asInt();
						deck.push_back(ParseCardInfo(entity_id, json_entities[entity_id], controllers));
					}

					Json::Value const& hand_entities = json["hand"]["entities"];
					hand.clear();
					for (Json::ArrayIndex idx = 0; idx < hand_entities.size(); ++idx) {
						int entity_id = hand_entities[idx].asInt();
						hand.push_back(ParseCardInfo(entity_id, json_entities[entity_id], controllers));
					}
				}

			private:
				CardInfo ParseCardInfo(int entity_id, Json::Value const& json_entity, Controllers & controllers) {
					CardInfo card_info;
					int controller = json_entity["controller"].asInt();

					std::string json_card_id = json_entity["card_id"].asString();

					int block_id = GetBlockIndex(json_entity["generate_under_blocks"]);
					auto block_cards_getter = [&]() {
						if (block_id == ControllerInfo::kDeckBlockId) {
							return controllers.Get(controller).deck_cards_;
						}
						else {
							return controllers.Get(controller).deck_cards_; // TODO: prepare cards according to block info
						}
					};

					if (!json_card_id.empty()) {
						Cards::CardId card_id = GetCardId(json_card_id);

						size_t unknown_cards_set_id = GetUnknownCardSetId(
							controller, block_id, block_cards_getter, controllers);
						controllers.Get(controller).unknown_cards_sets_.RemoveCardFromSet(
							unknown_cards_set_id, card_id);

						card_info.SetAsRevealedCard(entity_id, controller, card_id);
					}
					else {
						size_t unknown_cards_set_id = GetUnknownCardSetId(
							controller, block_id, block_cards_getter, controllers);
						size_t unknown_cards_set_card_idx = controllers.Get(controller)
							.unknown_cards_sets_.AssignCardToSet(unknown_cards_set_id);
						card_info.SetAsHiddenCard(entity_id, controller, unknown_cards_set_id, unknown_cards_set_card_idx);
					}

					return card_info;
				}

				int GetBlockIndex(Json::Value const& json_under_blocks) {
					if (json_under_blocks.size() == 0) return ControllerInfo::kDeckBlockId;
					else return json_under_blocks[json_under_blocks.size() - 1].asInt();
				}

				Cards::CardId GetCardId(std::string const& card_id)
				{
					auto const& container = Cards::Database::GetInstance().GetIdMap();
					auto it = container.find(card_id);
					if (it == container.end()) {
						return Cards::kInvalidCardId;
					}
					return (Cards::CardId)it->second;
				}

				template <class CardsGetter>
				size_t GetUnknownCardSetId(int controller, int block_idx, CardsGetter && cards_getter, Controllers & controllers)
				{
					auto & sets_indics = controllers.Get(controller).sets_indics_;
					auto it = sets_indics.find(block_idx);
					if (it == sets_indics.end()) {
						size_t set_idx = controllers.Get(controller).unknown_cards_sets_.AddCardsSet(cards_getter());
						sets_indics.insert(std::make_pair(block_idx, set_idx));
						return set_idx;
					}

					return it->second;
				}
			};
		}
	}
}