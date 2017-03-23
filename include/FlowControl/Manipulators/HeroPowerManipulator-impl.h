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
			state::CardRef new_ref = state_.AddCard(BoardManipulator(state_, flow_context_)
				.GenerateCardById(id, card_.GetPlayerIdentifier()));

			assert(state_.GetCard(new_ref).GetRawData().usable == true);

			state_.GetZoneChanger<state::kCardTypeHeroPower, state::kCardZonePlay>
				(flow_context_.GetRandom(), card_ref_)
				.ReplaceBy(new_ref);
		}
	}
}