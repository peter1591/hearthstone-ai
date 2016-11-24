#pragma once

#include <functional>
#include "State/CardRef.h"

namespace State
{
	class State;

	namespace Cards
	{
		class Card;
	}
}

using State::CardRef;

namespace FlowControl
{
	namespace Context
	{
		class BeforeMinionSummoned
		{
		public:
			typedef std::function<CardRef()> BattleCryTargetGetter;

			BeforeMinionSummoned(State::State & state, CardRef card_ref, const State::Cards::Card & card)
				: state_(state), card_ref_(card_ref), card_(card)
			{

			}

			State::State & GetState() { return state_; }
			CardRef GetCardRef() { return card_ref_; }
			const State::Cards::Card & GetCard() { return card_; }

		private:
			State::State & state_;
			CardRef card_ref_;
			const State::Cards::Card & card_;
		};
	}
}