#pragma once
#include "FlowControl/Manipulators/CardManipulator.h"

#include "state/State.h"
#include "Cards/CardDispatcher.h"
#include "MinionManipulator.h"

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

			card_.SetSilenced(state_.GetPlayOrder());
		}

		inline state::CardRef MinionManipulator::Transform(Cards::CardId id)
		{
			state::Cards::Card new_card = BoardManipulator(state_, flow_context_).GenerateCardById(id, card_.GetPlayerIdentifier());

			assert(card_.GetCardType() == state::kCardTypeMinion);
			assert(card_.GetZone() == state::kCardZonePlay);
			assert(new_card.GetCardType() == state::kCardTypeMinion);
			assert(new_card.GetZone() == state::kCardZoneNewlyCreated);

			state::CardRef new_card_ref = state_.AddCard(std::move(new_card));
			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeMinion>(Manipulate(state_, flow_context_), card_ref_)
				.ReplaceBy(new_card_ref);

			return new_card_ref;
		}

		inline state::CardRef MinionManipulator::BecomeCopyof(state::CardRef card_ref)
		{
			return BecomeCopyof(state_.GetCard(card_ref));
		}

		inline state::CardRef MinionManipulator::BecomeCopyof(state::Cards::Card const& card)
		{
			state::Cards::Card new_card = BoardManipulator(state_, flow_context_)
				.GenerateCardByCopy(card, card_.GetPlayerIdentifier());

			assert(card_.GetCardType() == state::kCardTypeMinion);
			assert(card_.GetZone() == state::kCardZonePlay);
			assert(new_card.GetCardType() == state::kCardTypeMinion);
			assert(new_card.GetZone() == state::kCardZoneNewlyCreated);

			state::CardRef new_card_ref = state_.AddCard(std::move(new_card));
			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeMinion>(Manipulate(state_, flow_context_), card_ref_)
				.ReplaceBy(new_card_ref);

			// recalculate enchantments from scratch
			// 'allow_hp_reduce' = true. <-- special case when transforming
			FlowControl::Manipulate(state_, flow_context_).Minion(new_card_ref)
				.Enchant().Update(true);

			return new_card_ref;
		}
	}
}