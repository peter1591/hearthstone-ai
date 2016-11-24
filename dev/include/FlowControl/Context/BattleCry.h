#pragma once

#include <functional>
#include "State/CardRef.h"

namespace State { class State; }
namespace Entity { class Card; }

namespace FlowControl
{
	namespace Context
	{
		class BattleCry
		{
		public:
			typedef std::function<CardRef()> BattleCryTargetGetter;

			BattleCry(State::State & state, CardRef card_ref, const Entity::Card & card, BattleCryTargetGetter battlecry_target_getter)
				: state_(state), card_ref_(card_ref), card_(card), battlecry_target_getter_(battlecry_target_getter)
			{

			}

			State::State & GetState() { return state_; }
			CardRef GetCardRef() { return card_ref_; }
			const Entity::Card & GetCard() { return card_; }
			CardRef GetBattleCryTarget() { return battlecry_target_getter_(); }

		private:
			State::State & state_;
			CardRef card_ref_;
			const Entity::Card & card_;
			BattleCryTargetGetter battlecry_target_getter_;
		};
	}
}