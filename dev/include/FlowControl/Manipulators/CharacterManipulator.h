#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "State/Cards/Manager.h"
#include "FlowControl/Manipulators/CardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		class CharacterManipulator : public CardManipulator
		{
		public:
			CharacterManipulator(state::State & state, CardRef card_ref, state::Cards::Card & card)
				: CardManipulator(state, card_ref, card)
			{
			}

			CharacterManipulator & TakeDamage(int damage)
			{
				card_.SetDamage(card_.GetDamage() + damage);
				return *this;
			}
		};
	}
}