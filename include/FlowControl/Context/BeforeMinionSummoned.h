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
		class BeforeMinionSummoned
		{
		public:
			typedef std::function<state::CardRef()> BattleCryTargetGetter;

			BeforeMinionSummoned(state::State & state, state::CardRef card_ref, const state::Cards::Card & card)
				: state_(state), card_ref_(card_ref), card_(card)
			{

			}

			state::State & GetState() { return state_; }
			state::CardRef GetCardRef() { return card_ref_; }
			const state::Cards::Card & GetCard() { return card_; }

		private:
			state::State & state_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};
	}
}