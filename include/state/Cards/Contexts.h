#pragma once

#include "state/Types.h"

namespace FlowControl { class Manipulate; }

namespace state
{
	namespace Cards
	{
		struct ZoneChangedContext {
			FlowControl::Manipulate & manipulate_;
			CardRef card_ref_;
			const Cards::Card & card_;
		};
	}
}