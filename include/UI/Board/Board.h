#pragma once

#include <string>
#include <vector>
#include <map>

#include <json/json.h>

#include <Cards/Database.h>
#include <Cards/id-map.h>
#include "UI/Board/UnknownCards.h"
#include "UI/Board/Entity.h"
#include "UI/Board/Player.h"

namespace ui
{
	namespace board
	{

		class Board
		{
		public:
			Board() :
				turn_(-1),
				first_player_(), second_player_()
			{

			}

			void Reset() {
				entities_.clear();
				ResetControllerInfo(first_controller_);
				ResetControllerInfo(second_controller_);
			}

			void SetDeckCards(int controller, std::vector<Cards::CardId> const& deck_cards)
			{
				GetControllerInfo(controller).deck_cards_ = deck_cards;
			}

			void Parse(Json::Value const& board)
			{
				turn_ = board["turn"].asInt();
				first_player_.Parse(board["player"]);
				second_player_.Parse(board["opponent"]);
				ParseEntities(board);
			}

		public:
			int GetTurn() const { return turn_; }
			auto const& GetFirstPlayer() const { return first_player_; }
			auto const& GetSecondPlayer() const { return second_player_; }
			
			// TODO: should return const&
			auto & GetUnknownCardsSets(int controller) {
				return GetControllerInfo(controller).unknown_cards_sets_;
			}

			Cards::CardId GetCardId(int entity_id, UnknownCardsSetsManager const& unknown_cards_mgr) const
			{
				Entity const& entity = entities_[entity_id];
				if (entity.card_id_ != Cards::kInvalidCardId) {
					return entity.card_id_;
				}
				else {
					return unknown_cards_mgr.GetCardId(
						entity.unknown_cards_set_id_,
						entity.unknown_cards_set_card_idx_);
				}
			}

		private:
			struct ControllerInfo {
				static constexpr int kDeckBlockId = -1;
				std::vector<Cards::CardId> deck_cards_;
				UnknownCardsSets unknown_cards_sets_;
				std::map<int, size_t> sets_indics_; // block id -> set idx
			};

		private:
			void ResetControllerInfo(ControllerInfo & info) {
				info.deck_cards_.clear();
				info.sets_indics_.clear();
				info.unknown_cards_sets_.Reset();
			}

			void ParseEntities(Json::Value const& board) {
				ParseEntitiesFromArray(board, board["player"]["hand"]["entities"]);
				ParseEntitiesFromArray(board, board["opponent"]["hand"]["entities"]);
				ParseEntitiesFromArray(board, board["player"]["deck"]["entities"]);
				ParseEntitiesFromArray(board, board["opponent"]["deck"]["entities"]);
			}

			void ParseEntitiesFromArray(Json::Value const& board, Json::Value const& json_entities) {
				for (Json::ArrayIndex idx = 0; idx < json_entities.size(); ++idx) {
					Entity & entity = ParseEntity(board, json_entities[idx].asInt());
					FillCardIdInfo(entity, board, board["entities"][entity.id_]);
				}
			}

			Entity & ParseEntity(Json::Value const& board, int entity_id) {
				Entity & entity = GetEntity(entity_id);
				Json::Value const& json_entity = board["entities"][entity_id];
				entity.id_ = entity_id;

				std::unordered_map<std::string, int> tags;
				Json::Value const& json_tags = json_entity["tags"];
				for (Json::ArrayIndex idx = 0; idx < json_tags.size(); ++idx) {
					tags[json_tags[idx]["Key"].asString()] = json_tags[idx]["Value"].asInt();
				}
				entity.ParseTags(tags);

				return entity;
			}

			void FillCardIdInfo(Entity & entity, Json::Value const& board, Json::Value const& json_entity) {
				entity.controller_ = json_entity["controller"].asInt();
				assert(entity.controller_ > 0);

				std::string json_card_id = json_entity["card_id"].asString();
				if (!json_card_id.empty()) {
					entity.card_id_ = GetCardId(json_card_id);
					if (entity.card_id_ == Cards::kInvalidCardId) assert(false);

					if (entity.card_id_ != Cards::ID_GAME_005) {
						entity.unknown_cards_set_id_ = GetUnknownCardSetId(
							board, entity.controller_, json_entity["generate_under_blocks"]);
						GetControllerInfo(entity.controller_).unknown_cards_sets_.RemoveCardFromSet(
							entity.unknown_cards_set_id_,
							entity.card_id_);
					}
					return;
				}

				entity.card_id_ = Cards::kInvalidCardId;
				entity.unknown_cards_set_id_ = GetUnknownCardSetId(
					board, entity.controller_, json_entity["generate_under_blocks"]);
				entity.unknown_cards_set_card_idx_ = GetControllerInfo(entity.controller_)
					.unknown_cards_sets_.AssignCardToSet(entity.unknown_cards_set_id_);
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

			Entity & GetEntity(int id)
			{
				if (id >= entities_.size()) {
					entities_.resize(id + 1);
				}
				return entities_[id];
			}

			size_t GetUnknownCardSetId(Json::Value const& board, int controller, Json::Value const& json_under_blocks)
			{
				if (json_under_blocks.size() == 0) {
					return GetUnknownCardSetId(board, controller, ControllerInfo::kDeckBlockId, [&]() {
						return GetControllerInfo(controller).deck_cards_;
					});
				}
				else {
					int block_id = json_under_blocks[json_under_blocks.size() - 1].asInt();
					return GetUnknownCardSetId(board, controller, block_id, [&]() {
						return GetControllerInfo(controller).deck_cards_; // TODO: prepare cards according to block info
					});
				}
			}

			template <class CardsGetter>
			size_t GetUnknownCardSetId(Json::Value const& board, int controller, int block_idx, CardsGetter && cards_getter)
			{
				auto & sets_indics = GetControllerInfo(controller).sets_indics_;
				auto it = sets_indics.find(block_idx);
				if (it == sets_indics.end()) {
					size_t set_idx = GetControllerInfo(controller).unknown_cards_sets_.AddCardsSet(cards_getter());
					sets_indics.insert(std::make_pair(block_idx, set_idx));
					return set_idx;
				}

				return it->second;
			}

			ControllerInfo & GetControllerInfo(int controller) {
				if (controller == 1) return first_controller_;
				else {
					assert(controller == 2);
					return second_controller_;
				}
			}

		private:
			int turn_;
			Player first_player_;
			Player second_player_;

			std::vector<Entity> entities_;

			ControllerInfo first_controller_;
			ControllerInfo second_controller_;
		};
	}
}