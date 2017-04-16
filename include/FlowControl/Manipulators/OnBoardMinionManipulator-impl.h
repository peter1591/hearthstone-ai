#pragma once
#include "FlowControl/Manipulators/OnBoardMinionManipulator.h"

#include "state/State.h"
#include "Cards/CardDispatcher.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void OnBoardMinionManipulator::Silence()
		{
			// No need to clear emitting aura. In next AuraUpdate(), the silenced flag is checked.

			// Remove all enchantments, including the aura enchantments coming from other minions.
			// Those aura enchantments will be added back in the next AuraUpdate()
			GetCard().GetMutableEnchantmentHandler().Silence();

			// Remove all deathrattles
			GetCard().GetMutableDeathrattleHandler().Clear();

			GetCard().SetTaunt(false);
			GetCard().SetShield(false);
			GetCard().SetCantAttack(false);
			GetCard().SetFreezed(false);

			GetCard().SetSilenced(state_.GetPlayOrder());
		}

		inline void OnBoardMinionManipulator::ChangeOwner(state::PlayerIdentifier new_owner)
		{
			assert(GetCard().GetPlayerIdentifier().Opposite() == new_owner);

			if (state_.GetBoard().Get(new_owner).minions_.Full()) return Destroy();

			int new_pos = (int)state_.GetBoard().Get(new_owner).minions_.Size();

			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeMinion>(
				Manipulate(state_, flow_context_), card_ref_)
				.ChangeTo<state::kCardZonePlay>(new_owner, new_pos);
		}

		inline state::CardRef OnBoardMinionManipulator::Transform(Cards::CardId id)
		{
			state::Cards::Card new_card = BoardManipulator(state_, flow_context_).GenerateCardById(id, GetCard().GetPlayerIdentifier());

			assert(GetCard().GetCardType() == state::kCardTypeMinion);
			assert(GetCard().GetZone() == state::kCardZonePlay);
			assert(new_card.GetCardType() == state::kCardTypeMinion);
			assert(new_card.GetZone() == state::kCardZoneNewlyCreated);

			state::CardRef new_card_ref = state_.AddCard(std::move(new_card));
			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeMinion>(Manipulate(state_, flow_context_), card_ref_)
				.ReplaceBy(new_card_ref);

			return new_card_ref;
		}

		inline state::CardRef OnBoardMinionManipulator::BecomeCopyof(state::CardRef card_ref)
		{
			return BecomeCopyof(state_.GetCard(card_ref));
		}

		inline state::CardRef OnBoardMinionManipulator::BecomeCopyof(state::Cards::Card const& card)
		{
			state::Cards::Card new_card = BoardManipulator(state_, flow_context_)
				.GenerateCardByCopy(card, GetCard().GetPlayerIdentifier());

			assert(GetCard().GetCardType() == state::kCardTypeMinion);
			assert(GetCard().GetZone() == state::kCardZonePlay);
			assert(new_card.GetCardType() == state::kCardTypeMinion);
			assert(new_card.GetZone() == state::kCardZoneNewlyCreated);

			state::CardRef new_card_ref = state_.AddCard(std::move(new_card));
			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeMinion>(Manipulate(state_, flow_context_), card_ref_)
				.ReplaceBy(new_card_ref);

			// recalculate enchantments from scratch
			// 'allow_hp_reduce' = true. <-- special case when transforming
			FlowControl::Manipulate(state_, flow_context_).OnBoardMinion(new_card_ref)
				.Enchant().Update(true);

			return new_card_ref;
		}
	}
}