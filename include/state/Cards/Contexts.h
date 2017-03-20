#pragma once

#include "state/Types.h"

namespace state
{
	class State;

	namespace Cards
	{
		struct ZoneChangedContext {
			State & state_;
			CardRef card_ref_;
			const Cards::Card & card_;
		};
	}
}