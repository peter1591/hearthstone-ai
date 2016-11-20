#pragma once

namespace State { class State; }
namespace Entity { class Card; }
class CardRef;

namespace FlowControl
{
	namespace Context
	{
		class BattleCry
		{
		public:
			BattleCry(State::State & state, CardRef card_ref, const Entity::Card & card)
				: state_(state), card_ref_(card_ref), card_(card)
			{

			}

			State::State & GetState() { return state_; }
			CardRef GetCardRef() { return card_ref_; }
			const Entity::Card & GetCard() { return card_; }

		private:
			State::State & state_;
			CardRef card_ref_;
			const Entity::Card & card_;
		};
	}
}