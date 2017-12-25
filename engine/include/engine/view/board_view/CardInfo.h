#pragma once

#include "Cards/id-map.h"

#include "engine/view/board_view/UnknownCards.h"

namespace engine
{
	namespace view
	{
		namespace board_view {
			class CardInfo
			{
			public:
				CardInfo() :
					id_(-1),
					card_id_(Cards::kInvalidCardId),
					unknown_cards_set_id_(0), unknown_cards_set_card_idx_(0)
				{}

				void SetAsRevealedCard(int id, Cards::CardId card_id) {
					assert(card_id != Cards::kInvalidCardId);

					id_ = id;
					card_id_ = card_id;
				}

				void SetAsHiddenCard(int id, size_t set_id, size_t card_idx) {
					id_ = id;
					card_id_ = Cards::kInvalidCardId;
					unknown_cards_set_id_ = set_id;
					unknown_cards_set_card_idx_ = card_idx;
				}

				Cards::CardId GetCardId(UnknownCardsSetsManager const& unknown_cards_mgr) const {
					if (card_id_ != Cards::kInvalidCardId) {
						return card_id_;
					}
					else {
						return unknown_cards_mgr.GetCardId(
							unknown_cards_set_id_,
							unknown_cards_set_card_idx_);
					}
				}

			public:
				int id_;

				Cards::CardId card_id_;
				size_t unknown_cards_set_id_;
				size_t unknown_cards_set_card_idx_;
			};
		}
	}
}