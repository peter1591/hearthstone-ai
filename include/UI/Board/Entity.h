#pragma once

#include "Cards/id-map.h"

#include "UI/Board/UnknownCards.h"

namespace ui
{
	namespace board
	{
		class Entity
		{
		public:
			Entity() : id_(-1),
				controller_(-1),
				card_id_(Cards::kInvalidCardId),
				unknown_cards_set_id_(0), unknown_cards_set_card_idx_(0)
			{}

			int id_;

			int controller_;
			Cards::CardId card_id_;
			size_t unknown_cards_set_id_;
			size_t unknown_cards_set_card_idx_;
		};
	}
}