#pragma once

#include "FlowControl/Manipulators/HeroManipulator.h"
#include "Cards/CardDispatcher.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void HeroManipulator::GainArmor(int amount)
		{
			GetCard().SetArmor(GetCard().GetArmor() + amount);

			// TODO: trigger events
		}

		inline state::CardRef HeroManipulator::Transform(Cards::CardId id)
		{
			assert(GetCard().GetCardType() == state::kCardTypeHero);
			assert(GetCard().GetZone() == state::kCardZonePlay);

			state::CardRef new_card_ref = BoardManipulator(state_, flow_context_).AddCardById(id, GetCard().GetPlayerIdentifier());
			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeHero>(Manipulate(state_, flow_context_), card_ref_)
				.ReplaceBy(new_card_ref);

			return new_card_ref;
		}
	}
}