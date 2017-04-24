#pragma once

#include "state/Types.h"
#include "state/targetor/TargetsGenerator.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace FlowControl
{
	class FlowContext;

	namespace onplay
	{
		namespace context
		{
			struct GetSpecifiedTarget {
				Manipulate & manipulate_;
				state::PlayerIdentifier player_;
				state::CardRef card_ref_;

				state::targetor::Targets * targets_;
				bool * allow_no_target_; // default: allow
				bool * need_to_prepare_target_; // default: need
			};

			struct OnPlay
			{
				Manipulate & manipulate_;
				state::PlayerIdentifier player_;
				state::CardRef card_ref_;
				state::CardRef * new_card_ref;

				state::CardRef GetTarget() const;
			};
		}
	}
}