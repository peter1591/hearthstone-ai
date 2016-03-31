#pragma once

#include "card.h"

namespace GameEngine
{
	class HandCard
	{
	public:
		enum Type {
			TYPE_DETERMINED,
			TYPE_DRAW_FROM_HIDDEN_CARDS, // draw from the hidden cards in hand and deck
		};

	public: // cannot be compared as normal
		bool operator==(HandCard const& rhs) const = delete;
		bool operator!=(HandCard const& rhs) const = delete;

	public:
		Type type;
		Card card;
	};
} // namespace GameEngine