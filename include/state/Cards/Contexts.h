#pragma once

#include "state/Types.h"
#include "FlowControl/Contexts.h"

namespace state
{
	class State;

	namespace Cards
	{
		struct ZoneChangedContext {
			state::State & state_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};
	}
}