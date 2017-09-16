#pragma once

#include "Cards/id-map.h"

namespace ui
{
	namespace board
	{
		class Entity
		{
		public:
			int id_;

			Cards::CardId card_id_;
			size_t unknown_cards_set_id_;
		};
	}
}