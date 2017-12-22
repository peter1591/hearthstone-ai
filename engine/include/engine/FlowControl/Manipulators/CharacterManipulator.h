#pragma once

#include "state/Types.h"
#include "state/Cards/Card.h"
#include "engine/FlowControl/Manipulators/CardManipulator.h"
#include "engine/FlowControl/Manipulators/Helpers/DamageHelper.h"

namespace engine {
	namespace FlowControl
	{
		namespace Helpers { class DamageDealer; }

		namespace Manipulators
		{
			class CharacterManipulator : public CardManipulator
			{
			public:
				CharacterManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref)
					: CardManipulator(state, flow_context, card_ref)
				{
					assert(GetCard().GetCardType() == state::kCardTypeMinion ||
						GetCard().GetCardType() == state::kCardTypeHero);
				}

				void Taunt(bool v) { GetCard().SetTaunt(v); }
				void Shield(bool v) { GetCard().SetShield(v); }
				void Charge(bool v) { GetCard().SetCharge(v); }
				void Stealth(bool v) { GetCard().SetStealth(v); }
				void Freeze(bool v) { GetCard().SetFreezed(v); }
			};
		}
	}
}