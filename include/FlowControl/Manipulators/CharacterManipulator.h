#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "State/Cards/Manager.h"
#include "FlowControl/Manipulators/CardManipulator.h"
#include "FlowControl/Manipulators/Helpers/DamageHelper.h"

namespace FlowControl
{
	namespace Helpers { class DamageDealer; }

	namespace Manipulators
	{
		class CharacterManipulator : public CardManipulator
		{
		public:
			CharacterManipulator(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
				: CardManipulator(state, flow_context, card_ref, card)
			{
			}

			detail::DamageSetter Internal_SetDamage() { return detail::DamageSetter(card_); }

			void AfterAttack()
			{
				card_.IncreaseNumAttacksThisTurn();
			}
		};
	}
}