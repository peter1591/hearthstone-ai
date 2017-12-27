#pragma once

#include <random>
#include <sstream>
#include <string>
#include <shared_mutex>

#include <json/json.h>

#include "state/Configs.h"
#include "Cards/Database.h"
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

					board.Reset();

					//std::string player_deck_type = "InnKeeperBasicMage"; // TODO: use correct deck
					std::string player_deck_type = "InnKeeperExpertWarlock"; // TODO: use correct deck
					// TODO: remove revealed deck cards
					board.SetDeckCards(state::kPlayerFirst, player_deck_type);

					// TODO: guess oppoennt deck type
					//std::string opponent_deck_type = "InnKeeperBasicMage";
					std::string opponent_deck_type = "InnKeeperExpertWarlock"; // TODO: use correct deck
					// TODO: remove revealed deck cards
					board.SetDeckCards(state::kPlayerSecond, player_deck_type);

					board.Parse(json_board);
				}
			};
		}
	}
}