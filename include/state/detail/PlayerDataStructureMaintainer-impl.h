#pragma once

#include <assert.h>
#include "state/State.h"
#include "PlayerDataStructureMaintainer.h"

namespace state {
	namespace detail {
		template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer;

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneInvalid>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneNewlyCreated);
			assert(false);
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneInvalid>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneNewlyCreated);
			assert(false);
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneNewlyCreated>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate, CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneNewlyCreated);
			return; // do nothing
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneNewlyCreated>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate, CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneNewlyCreated);
			return; // do nothing
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneRemoved);
			return; // do nothing
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneRemoved);
			return; // do nothing
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneSetASide);
			return; // do nothing
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneSetASide);
			return; // do nothing
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneDeck>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneDeck);
			board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).deck_.ShuffleAdd(card_ref, [&manipulate](auto exclusive_max) {
				return manipulate.GetRandom().Get(exclusive_max);
			});
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneDeck>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneDeck);
			board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).deck_.RemoveLast();
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneGraveyard);
			board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).graveyard_.Add<TargetCardType>(card_ref);
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneGraveyard);
			board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).graveyard_.Remove<TargetCardType>(card_ref);
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneHand);
			int pos = (int)board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).hand_.PushBack(card_ref);
			cards_mgr.GetMutable(card_ref).SetZonePos()(pos);
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == TargetCardType);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZoneHand);

			int pos = cards_mgr.Get(card_ref).GetZonePosition();
			assert(board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).hand_.Get(pos) == card_ref);
			auto& hand = board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).hand_;
			hand.Remove(pos, [&cards_mgr](CardRef ref, size_t pos) {
				cards_mgr.SetCardZonePos(ref, (int)pos);
			});
		}

		inline void PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeHero);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			board::Player & player = board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier());
			if (player.GetHeroRef().IsValid()) throw std::exception("hero should be removed first");
			player.SetHeroRef(card_ref);
		}
		inline void PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeHero);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			board::Player & player = board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier());
			player.GetHeroRef().Invalidate();
		}

		inline void PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref, int pos)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeMinion);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);

			board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).minions_.Insert(card_ref, pos, [&cards_mgr](CardRef ref, size_t pos)
			{
				cards_mgr.SetCardZonePos(ref, (int)pos);
			});
		}
		inline void PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeMinion);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);

			board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).minions_.Remove(
				cards_mgr.Get(card_ref).GetZonePosition(),
				[&cards_mgr](CardRef ref, size_t pos) {
				cards_mgr.SetCardZonePos(ref, (int)pos);
			});
		}
		inline void PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay>::
			ReplaceBy(board::Board & board, Cards::Manager & cards_mgr, CardRef card_ref, CardRef new_card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeMinion);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);

			board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).minions_.Replace(cards_mgr.Get(card_ref).GetZonePosition(), new_card_ref);
		}

		inline void PlayerDataStructureMaintainer<kCardTypeSecret, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeSecret);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			board::Player & player = board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier());
			return player.secrets_.Add(cards_mgr.Get(card_ref).GetCardId(), card_ref);
		}
		inline void PlayerDataStructureMaintainer<kCardTypeSecret, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeSecret);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			board::Player & player = board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier());
			return player.secrets_.Remove(cards_mgr.Get(card_ref).GetCardId());
		}

		inline void PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeWeapon);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			board::Player & player = board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier());
			player.SetWeaponRef(card_ref);
		}
		inline void PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeWeapon);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			board::Player & player = board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier());
			player.InvalidateWeaponRef();
		}

		inline void PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeSpell);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			// do nothing
		}
		inline void PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeSpell);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			// do nothing
		}

		inline void PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeHeroPower);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			cards_mgr.GetMutable(card_ref).SetUsable();
			board::Player & player = board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier());
			assert(player.hero_power_ref_.IsValid() == false);
			player.SetHeroPowerRef(card_ref);
		}
		inline void PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeHeroPower);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			board::Player & player = board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier());
			player.InvalidateHeroPowerRef();
		}
		inline void PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay>::
			ReplaceBy(board::Board & board, Cards::Manager & cards_mgr, CardRef card_ref, CardRef new_card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeHeroPower);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);

			board.Get(cards_mgr.Get(card_ref).GetPlayerIdentifier()).hero_power_ref_ = new_card_ref;
		}

		inline void PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeEnchantment);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			// do nothing
		}
		inline void PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref)
		{
			assert(cards_mgr.Get(card_ref).GetCardType() == kCardTypeEnchantment);
			assert(cards_mgr.Get(card_ref).GetZone() == kCardZonePlay);
			// do nothing
		}
	}
}