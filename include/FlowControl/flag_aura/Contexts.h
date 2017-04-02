#pragma once

#include "state/Types.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace FlowControl
{
	class FlowContext;

	namespace flag_aura
	{
		class Handler;

		namespace contexts {
			struct AuraIsValid
			{
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				flag_aura::Handler const& aura_handler_; // TODO: this might be invalidated if flag_aura's container got expanded. Unless we can ensure the callback will not added a new entry to flag-aura manager
			};

			struct AuraApplyRemove
			{
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				flag_aura::Handler & handler_; // TODO: this might be invalidated if flag_aura's container got expanded. Unless we can ensure the callback will not added a new entry to flag-aura manager
			};
			
			typedef AuraApplyRemove AuraApply;
			typedef AuraApplyRemove AuraRemove;
		}
	}
}