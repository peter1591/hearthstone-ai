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
			// TODO: move this to ui module
			// Thread safety: No
			class Parser
			{
			public:
				void Parse(std::string const& board_raw, BoardView & board) {
					Json::Reader reader;
					Json::Value json_board;
					std::stringstream ss(board_raw);
					if (!reader.parse(ss, json_board)) {
						throw std::runtime_error("failed to parse board");
					}

					int player_entity_id = json_board["player"]["entity_id"].asInt();
					int opponent_entity_id = json_board["opponent"]["entity_id"].asInt();

					board.Reset();

					//std::string player_deck_type = "InnKeeperBasicMage"; // TODO: use correct deck
					std::string player_deck_type = "InnKeeperExpertWarlock"; // TODO: use correct deck
					std::vector<Cards::CardId> player_deck_cards;
					for (auto const& card_name : decks::Decks::GetDeck(player_deck_type)) {
						Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
						player_deck_cards.push_back(card_id);
					}
					// TODO: remove revealed deck cards
					board.SetDeckCards(state::kPlayerFirst, player_deck_cards);

					// TODO: guess oppoennt deck type
					//std::string opponent_deck_type = "InnKeeperBasicMage";
					std::string opponent_deck_type = "InnKeeperExpertWarlock"; // TODO: use correct deck
					std::vector<Cards::CardId> opponent_deck_cards;
					for (auto const& card_name : decks::Decks::GetDeck(opponent_deck_type)) {
						Cards::CardId card_id = (Cards::CardId)Cards::Database::GetInstance().GetIdByCardName(card_name);
						opponent_deck_cards.push_back(card_id);
					}
					board.SetDeckCards(state::kPlayerSecond, opponent_deck_cards);

					board.Parse(json_board);
				}
			};
		}
	}
}