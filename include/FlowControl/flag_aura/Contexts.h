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
				flag_aura::Handler const& aura_handler_;
			};

			struct AuraApplyRemove
			{
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				flag_aura::Handler & handler_;
			};
			
			typedef AuraApplyRemove AuraApply;
			typedef AuraApplyRemove AuraRemove;
		}
	}
}