#pragma once

#include "state/Types.h"
#include "state/targetor/Targets.h"
#include "FlowControl/enchantment/Enchantments.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace FlowControl
{
	class Manipulate;

	namespace aura
	{
		class Handler;

		namespace contexts {
			struct AuraIsValid
			{
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				aura::Handler const& aura_handler_;
				bool & need_update_;
			};

			struct AuraGetTargets
			{
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				std::unordered_set<state::CardRef> & new_targets;
			};

			struct AuraGetTarget
			{
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				state::CardRef & new_target;
			};

			struct AuraApplyOn
			{
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				state::CardRef target_;
			};
		}
	}
}