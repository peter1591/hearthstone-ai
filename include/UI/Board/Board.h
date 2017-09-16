#pragma once

#include <string>
#include <vector>
#include <map>

#include <json/json.h>

#include <Cards/id-map.h>
#include "UI/Board/UnknownCards.h"
#include "UI/Board/Entity.h"

namespace ui
{
	namespace board
	{
		class Board
		{
		public:
			void SetDeckCards(std::vector<Cards::CardId> const& deck_cards)
			{
				deck_cards_ = deck_cards;
			}

			void Parse(Json::Value const& board)
			{
				ParseEntities(board);

				// TODO: define a board structure, and read json to there
				// board structure:
				//   blocks:
				//   entities:
				//   player:
				//      hand:
				//          0: entity_id
				//          1: entity_id
				//      deck:
				//          0: entity_id
				//      minions:
				//          0: entity_id
				//          1: entity_id
				//   opponent:
				//       (similar)
			}

		private:
			void ParseEntities(Json::Value const& board) {
				Json::Value const& entities = board["entities"];
				for (Json::ArrayIndex idx = 0; idx < entities.size(); ++idx) {
					ParseEntity(board, entities[idx]);
				}
			}

			void ParseEntity(Json::Value const& board, Json::Value const& json_entity) {
				int id = json_entity["id"].asInt();
				Entity & entity = GetEntity(id);
				entity.id_ = id;
				FillCardIdInfo(entity, board, json_entity);
			}

			void FillCardIdInfo(Entity & entity, Json::Value const& board, Json::Value const& json_entity) {
				std::string json_card_id = json_entity["card_id"].asString();
				if (!json_card_id.empty()) {
					entity.card_id_ = GetCardId(json_card_id);
					if (entity.card_id_ != Cards::kInvalidCardId) {
						entity.unknown_cards_set_id_ = -1;
						return;
					}
				}
				entity.card_id_ = Cards::kInvalidCardId;
				entity.unknown_cards_set_id_ = GetUnknownCardSetId(
					board, json_entity["generate_under_blocks"]);
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

			size_t GetUnknownCardSetId(Json::Value const& board, Json::Value const& json_under_blocks)
			{
				if (json_under_blocks.size() == 0) {
					return GetUnknownCardSetId(board, kDeckBlockId, [&]() {
						return deck_cards_;
					});
				}
				else {
					int block_id = json_under_blocks[json_under_blocks.size() - 1].asInt();
					return GetUnknownCardSetId(board, block_id, [&]() {
						return deck_cards_; // TODO: prepare cards according to block info
					});
				}
			}

			template <class CardsGetter>
			size_t GetUnknownCardSetId(Json::Value const& board, int block_idx, CardsGetter && cards_getter)
			{
				auto it = sets_indics_.find(block_idx);
				if (it == sets_indics_.end()) {
					size_t set_idx = unknown_cards_sets_.AddCardsSet(cards_getter());
					sets_indics_.insert(std::make_pair(block_idx, set_idx));
					return set_idx;
				}

				return it->second;
			}

		private:
			static constexpr int kDeckBlockId = -1;

			std::vector<Entity> entities_;
			std::vector<Cards::CardId> deck_cards_;
			UnknownCardsSets unknown_cards_sets_;
			std::map<int, size_t> sets_indics_; // block id -> set idx
		};
	}
}