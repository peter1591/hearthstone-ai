#pragma once

#include <functional>
#include "State/Types.h"
#include "FlowControl/FlowContext.h"

namespace state
{
	class State;
	namespace Cards
	{
		class Card;
	}
}

namespace FlowControl
{
	namespace Context
	{
		class BattleCry
		{
		public:
			typedef std::function<state::CardRef(std::vector<state::CardRef> const&)> BattleCryTargetGetter;

			BattleCry(state::State & state, FlowContext & flow_context, state::CardRef card_ref, const state::Cards::Card & card, BattleCryTargetGetter battlecry_target_getter)
				: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card), battlecry_target_getter_(battlecry_target_getter)
			{

			}

			state::CardRef GetBattleCryTarget(std::vector<state::CardRef> const& targets) { return battlecry_target_getter_(targets); }

			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;

		private:
			BattleCryTargetGetter battlecry_target_getter_;
		};
	}
}