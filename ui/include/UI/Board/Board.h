#pragma once

#include <string>
#include <vector>
#include <map>

#include <json/json.h>

#include <Cards/Database.h>
#include <Cards/id-map.h>
#include "UI/Board/UnknownCards.h"
#include "UI/Board/CardInfo.h"
#include "UI/Board/Player.h"

namespace ui
{
	namespace board
	{
		class Board
		{
		public:
			Board() :
				turn_(-1), first_player_(), second_player_(),
				cards_info_(), first_controller_(), second_controller_()
			{

			}

			void Reset() {
				turn_ = -1;
				cards_info_.clear();
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

			// TODO: should be private?
			Cards::CardId GetCardId(int card_info_id, UnknownCardsSetsManager const& unknown_cards_mgr) const
			{
				CardInfo const& card_info = cards_info_[card_info_id];
				return card_info.GetCardId(unknown_cards_mgr);
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
					int entity_id = json_entities[idx].asInt();
					FillCardIdInfo(entity_id, board, board["entities"][entity_id]);
				}
			}

			void FillCardIdInfo(int entity_id, Json::Value const& board, Json::Value const& json_entity) {
				auto & card_info = GetCardInfo(entity_id);
				int controller = json_entity["controller"].asInt();

				std::string json_card_id = json_entity["card_id"].asString();
				if (!json_card_id.empty()) {
					Cards::CardId card_id = GetCardId(json_card_id);

					size_t unknown_cards_set_id = GetUnknownCardSetId(
						board, controller, json_entity["generate_under_blocks"]);
					GetControllerInfo(controller).unknown_cards_sets_.RemoveCardFromSet(
						unknown_cards_set_id, card_id);

					card_info.SetAsRevealedCard(entity_id, controller, card_id);
				}
				else {
					size_t unknown_cards_set_id = GetUnknownCardSetId(
						board, controller, json_entity["generate_under_blocks"]);
					size_t unknown_cards_set_card_idx = GetControllerInfo(controller)
						.unknown_cards_sets_.AssignCardToSet(unknown_cards_set_id);
					card_info.SetAsHiddenCard(entity_id, controller, unknown_cards_set_id, unknown_cards_set_card_idx);
				}
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

			CardInfo & GetCardInfo(int id)
			{
				if (id >= cards_info_.size()) {
					cards_info_.resize(id + 1);
				}
				return cards_info_[id];
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

			std::vector<CardInfo> cards_info_;

			ControllerInfo first_controller_;
			ControllerInfo second_controller_;
		};
	}
}