#pragma once

#include <string>
#include <vector>
#include <map>

#include <json/json.h>

#include "Cards/id-map.h"
#include "decks/Decks.h"
#include "engine/view/board_view/UnknownCards.h"
#include "engine/view/board_view/CardInfo.h"
#include "engine/view/board_view/Player.h"

namespace engine
{
	namespace view
	{
		class BoardView
		{
		public:
			BoardView() : turn_(-1), first_player_(), second_player_(), cards_info_() {}

			void Reset() {
				turn_ = -1;
				cards_info_.clear();
			}

			void Parse(Json::Value const& board, board_view::UnknownCardsInfo & first_unknown, board_view::UnknownCardsInfo & second_unknown)
			{
				turn_ = board["turn"].asInt();

				// TODO: make this more clearer. should this be here?
				current_player_ = state::kPlayerFirst; // AI is helping first player, and should now waiting for an action
				
				first_player_.Parse(board["player"], board["entities"], first_unknown);
				second_player_.Parse(board["opponent"], board["entities"], second_unknown);
			}

			void Parse(BoardRefView game_state, board_view::UnknownCardsInfo & first_unknown, board_view::UnknownCardsInfo & second_unknown)
			{
				turn_ = game_state.GetTurn();
				current_player_ = game_state.GetCurrentPlayer();
				first_player_.Parse(game_state, state::kPlayerFirst, first_unknown);
				second_player_.Parse(game_state, state::kPlayerSecond, second_unknown);
			}

		public:
			int GetTurn() const { return turn_; }
			auto GetCurrentPlayer() const { return current_player_; }

			board_view::Player const& GetFirstPlayer() const { return first_player_; }
			board_view::Player const& GetSecondPlayer() const { return second_player_; }

		private:
			board_view::Player & GetPlayer(state::PlayerSide side) {
				if (side == state::kPlayerFirst) return first_player_;
				else return second_player_;
			}

		private:
			board_view::CardInfo & GetCardInfo(int id)
			{
				if (id >= cards_info_.size()) {
					cards_info_.resize(id + 1);
				}
				return cards_info_[id];
			}

		private:
			int turn_;
			state::PlayerIdentifier current_player_;
			board_view::Player first_player_;
			board_view::Player second_player_;

			std::vector<board_view::CardInfo> cards_info_;
		};
	}
}