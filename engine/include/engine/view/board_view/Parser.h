#pragma once

#include <random>
#include <sstream>
#include <string>
#include <shared_mutex>

#include <json/json.h>

#include "state/Configs.h"
#include "Cards/Database.h"
#include "decks/Decks.h"
#include "engine/view/BoardView.h"
#include "engine/view/board_view/UnknownCards.h"
#include "engine/view/board_view/StateRestorer.h"

namespace engine
{
	namespace view
	{
		namespace board_view {
			// Thread safety: No
			class Parser
			{
			public:
				Parser() : board_() {}

				StateRestorer Parse(std::string const& board_raw) {
					Json::Reader reader;
					Json::Value json_board;
					std::stringstream ss(board_raw);
					if (!reader.parse(ss, json_board)) {
						throw std::runtime_error("failed to parse board");
					}

					int player_entity_id = json_board["player"]["entity_id"].asInt();
					int opponent_entity_id = json_board["opponent"]["entity_id"].asInt();

					board_.Reset();

					//std::string player_deck_type = "InnKeeperBasicMage"; // TODO: use correct deck
					std::string player_deck_type = "InnKeeperExpertWarlock"; // TODO: use correct deck
					std::vector<Cards::CardId> player_deck_cards;
					for (auto const& card_name : decks::Decks::GetDeck(player_deck_type)) {
						Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
						player_deck_cards.push_back(card_id);
					}
					// TODO: remove revealed deck cards
					board_.SetDeckCards(1, player_deck_cards);

					// TODO: guess oppoennt deck type
					//std::string opponent_deck_type = "InnKeeperBasicMage";
					std::string opponent_deck_type = "InnKeeperExpertWarlock"; // TODO: use correct deck
					std::vector<Cards::CardId> opponent_deck_cards;
					for (auto const& card_name : decks::Decks::GetDeck(opponent_deck_type)) {
						Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
						opponent_deck_cards.push_back(card_id);
					}
					board_.SetDeckCards(2, opponent_deck_cards);

					board_.Parse(json_board);
					first_unknown_cards_sets_mgr_.Setup(board_.GetUnknownCardsSets(1));
					second_unknown_cards_sets_mgr_.Setup(board_.GetUnknownCardsSets(2));

					StateRestorer state_restorer(board_, first_unknown_cards_sets_mgr_, second_unknown_cards_sets_mgr_);
					return state_restorer;
				}

			private:
				BoardView board_;
				board_view::UnknownCardsSetsManager first_unknown_cards_sets_mgr_;
				board_view::UnknownCardsSetsManager second_unknown_cards_sets_mgr_;
			};
		}
	}
}