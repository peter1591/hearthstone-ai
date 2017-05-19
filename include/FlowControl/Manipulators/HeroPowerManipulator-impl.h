#pragma once

#include "FlowControl/Manipulators/HeroPowerManipulator.h"

#include "Cards/CardDispatcher.h"
#include "FlowControl/Manipulators/BoardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void HeroPowerManipulator::ReplaceHeroPower(Cards::CardId id)
		{
			// This interface is not located in HeroManipulator because:
			//    The hero-power-ref is recorded in Board, and its automatically maintained by state_.GetZoneChanger()

			state::CardRef new_ref = BoardManipulator(state_, flow_context_).AddCardById(id, GetCard().GetPlayerIdentifier());

			assert(state_.GetCard(new_ref).GetRawData().usable == true);

			state_.GetZoneChanger<state::kCardTypeHeroPower, state::kCardZonePlay>
				(Manipulate(state_, flow_context_), card_ref_)
				.ReplaceBy(new_ref);
		}
	}
}