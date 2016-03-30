#pragma once

#include "card.h"

namespace GameEngine
{
	class HandCard
	{
	public:
		enum Type {
			TYPE_DETERMINED
		};

	public: // cannot be compared as normal
		bool operator==(HandCard const& rhs) const = delete;
		bool operator!=(HandCard const& rhs) const = delete;

	public:
		Type type;
		Card card;
	};
} // namespace GameEngine