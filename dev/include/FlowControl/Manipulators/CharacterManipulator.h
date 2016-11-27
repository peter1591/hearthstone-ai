#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "State/Cards/Manager.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class CharacterManipulator
		{
		public:
			CharacterManipulator(state::State & state, CardRef card_ref, state::Cards::Card & card)
				: state_(state), card_(card), card_ref_(card_ref)
			{
			}

			CharacterManipulator & TakeDamage(int damage)
			{
				card_.SetDamage(card_.GetDamage() + damage);
				return *this;
			}

		private:
			state::State & state_;
			state::Cards::Card & card_;
			CardRef card_ref_;
		};
	}
}