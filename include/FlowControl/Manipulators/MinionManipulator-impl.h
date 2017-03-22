#pragma once
#include "FlowControl/Manipulators/CardManipulator.h"

#include "state/State.h"
#include "Cards/CardDispatcher.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void MinionManipulator::Silence()
		{
			// No need to clear emitting aura. In next AuraUpdate(), the silenced flag is checked.

			// Remove all enchantments, including the aura enchantments coming from other minions.
			// Those aura enchantments will be added back in the next AuraUpdate()
			card_.GetMutableEnchantmentHandler().Clear();

			// Remove all deathrattles
			card_.GetMutableDeathrattleHandler().Clear();

			card_.SetSilenced();
		}

		inline void MinionManipulator::Transform(Cards::CardId id)
		{
			state::Cards::CardData new_data = BoardManipulator(state_, flow_context_).GenerateCard(id, card_.GetPlayerIdentifier());

			assert(card_.GetCardType() == state::kCardTypeMinion);
			assert(card_.GetZone() == state::kCardZonePlay);
			assert(new_data.card_type == state::kCardTypeMinion);
			assert(new_data.zone == state::kCardZoneNewlyCreated);

			state::CardRef new_card_ref = state_.AddCard(state::Cards::Card(std::move(new_data)));
			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeMinion>(flow_context_.GetRandom(), card_ref_)
				.ReplaceBy(new_card_ref);
		}
	}
}