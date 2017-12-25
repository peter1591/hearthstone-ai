#pragma once

#include <string>
#include <vector>
#include <map>

#include <json/json.h>

#include <Cards/id-map.h>
#include "engine/view/board_view/UnknownCards.h"
#include "engine/view/board_view/CardInfo.h"
#include "engine/view/board_view/Player.h"
#include "engine/view/board_view/Controllers.h"

namespace engine
{
	namespace view
	{
		class BoardView
		{
		public:
			BoardView() :
				turn_(-1), first_player_(), second_player_(),
				cards_info_(), controllers_()
			{

			}

			void Reset() {
				turn_ = -1;
				cards_info_.clear();
				controllers_.Reset();
			}

			void SetDeckCards(int controller, std::vector<Cards::CardId> const& deck_cards)
			{
				controllers_.Get(controller).deck_cards_ = deck_cards;
			}

			void Parse(Json::Value const& board)
			{
				turn_ = board["turn"].asInt();
				first_player_.Parse(board["player"], board["entities"], controllers_);
				second_player_.Parse(board["opponent"], board["entities"], controllers_);
			}

		public:
			int GetTurn() const { return turn_; }
			auto const& GetFirstPlayer() const { return first_player_; }
			auto const& GetSecondPlayer() const { return second_player_; }
			
			// TODO: should return const&
			auto & GetUnknownCardsSets(int controller) {
				return controllers_.Get(controller).unknown_cards_sets_;
			}

			// TODO: should be private?
			Cards::CardId GetCardIdA(int card_info_id, board_view::UnknownCardsSetsManager const& unknown_cards_mgr) const
			{
				board_view::CardInfo const& card_info = cards_info_[card_info_id];
				return card_info.GetCardId(unknown_cards_mgr);
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
			board_view::Player first_player_;
			board_view::Player second_player_;

			std::vector<board_view::CardInfo> cards_info_;

			board_view::Controllers controllers_;
		};
	}
}