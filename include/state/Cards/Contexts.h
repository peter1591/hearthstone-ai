#pragma once

#include "state/Types.h"

namespace FlowControl { class FlowContext; }
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

		struct GetSpecifiedTargetContext {
			State & state_;
			FlowControl::FlowContext & flow_context_;
			CardRef card_ref_;
			const Cards::Card & card_;
		};
	}
}