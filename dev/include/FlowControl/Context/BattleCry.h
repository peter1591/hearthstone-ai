#pragma once

#include <functional>
#include "State/Types.h"

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
			typedef std::function<CardRef()> BattleCryTargetGetter;

			BattleCry(state::State & state, CardRef card_ref, const state::Cards::Card & card, BattleCryTargetGetter battlecry_target_getter)
				: state_(state), card_ref_(card_ref), card_(card), battlecry_target_getter_(battlecry_target_getter)
			{

			}

			state::State & GetState() { return state_; }
			CardRef GetCardRef() { return card_ref_; }
			const state::Cards::Card & GetCard() { return card_; }
			CardRef GetBattleCryTarget() { return battlecry_target_getter_(); }

		private:
			state::State & state_;
			CardRef card_ref_;
			const state::Cards::Card & card_;
			BattleCryTargetGetter battlecry_target_getter_;
		};
	}
}