#pragma once

#include <functional>
#include <unordered_set>
#include "state/Types.h"
#include "state/Cards/Enchantments/Enchantments.h"

namespace state
{
	class State;
	namespace Cards {
		class Card;
		namespace aura { class AuraAuxData; }
	}
	namespace targetor {
		class TargetsGenerator;
	}
}

namespace FlowControl
{
	class FlowContext;
	namespace Context
	{
		struct BattleCry
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;

			state::CardRef GetTarget();
		};

		struct BattlecryTargetGetter
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
			state::targetor::TargetsGenerator & targets_generator_;

			state::targetor::TargetsGenerator & Targets() { return targets_generator_; }
		};

		struct Deathrattle
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};
	}
}