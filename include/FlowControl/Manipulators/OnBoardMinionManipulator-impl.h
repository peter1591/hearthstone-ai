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
			if (GetCard().GetZone() != state::kCardZonePlay) return;

			// No need to clear emitting aura. In next AuraUpdate(), the silenced flag is checked.

			// Remove all enchantments, including the aura enchantments coming from other minions.
			// Those aura enchantments will be added back in the next AuraUpdate()
			GetCard().GetMutableEnchantmentHandler().Silence();

			// Remove all deathrattles
			GetCard().GetMutableDeathrattleHandler().Clear();

			static_assert(state::Cards::CardData::kFieldChangeId == 2);
			GetCard().SetTaunt(false);
			GetCard().SetShield(false);
			GetCard().SetCantAttack(false);
			GetCard().SetFreezed(false);
			GetCard().SetCantAttackHero(false);

			GetCard().SetSilenced();
		}

		inline void OnBoardMinionManipulator::ChangeOwner(state::PlayerIdentifier new_owner)
		{
			if (GetCard().GetZone() != state::kCardZonePlay) return;
			assert(GetCard().GetPlayerIdentifier().Opposite() == new_owner);

			int new_pos = (int)state_.GetBoard().Get(new_owner).minions_.Size();
			return MinionManipulator<state::kCardZonePlay>(state_, flow_context_, card_ref_)
				.MoveTo<state::kCardZonePlay>(new_owner, new_pos);
		}

		inline state::CardRef OnBoardMinionManipulator::Transform(Cards::CardId id)
		{
			if (GetCard().GetZone() != state::kCardZonePlay) return state::CardRef();

			assert(GetCard().GetCardType() == state::kCardTypeMinion);
			assert(GetCard().GetZone() == state::kCardZonePlay);

			state::CardRef new_card_ref = BoardManipulator(state_, flow_context_).AddCardById(id, GetCard().GetPlayerIdentifier());
			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeMinion>(card_ref_)
				.ReplaceBy(new_card_ref);

			return new_card_ref;
		}

		inline state::CardRef OnBoardMinionManipulator::BecomeCopyof(state::CardRef card_ref)
		{
			return BecomeCopyof(state_.GetCard(card_ref));
		}

		inline state::CardRef OnBoardMinionManipulator::BecomeCopyof(state::Cards::Card const& card)
		{
			if (GetCard().GetZone() != state::kCardZonePlay) return state::CardRef();

			assert(GetCard().GetCardType() == state::kCardTypeMinion);
			assert(GetCard().GetZone() == state::kCardZonePlay);

			state::CardRef new_card_ref = BoardManipulator(state_, flow_context_)
				.AddCardByCopy(card, GetCard().GetPlayerIdentifier());
			assert(state_.GetCard(new_card_ref).GetCardType() == state::kCardTypeMinion);
			assert(state_.GetCard(new_card_ref).GetZone() == state::kCardZoneNewlyCreated);

			state_.GetZoneChanger<state::kCardZonePlay, state::kCardTypeMinion>(card_ref_)
				.ReplaceBy(new_card_ref);

			// recalculate enchantments from scratch
			// 'allow_hp_reduce' = true. <-- special case when transforming
			FlowControl::Manipulate(state_, flow_context_).OnBoardMinion(new_card_ref)
				.Enchant().Update(true);

			return new_card_ref;
		}

		inline void OnBoardMinionManipulator::AddDeathrattle(FlowControl::deathrattle::Handler::DeathrattleCallback * deathrattle)
		{
			if (GetCard().GetZone() != state::kCardZonePlay) return;
			state_.GetMutableCard(card_ref_).GetMutableDeathrattleHandler().Add(deathrattle);
		}
	}
}