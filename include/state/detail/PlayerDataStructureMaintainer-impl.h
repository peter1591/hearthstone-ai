#pragma once

#include <assert.h>
#include "state/State.h"

namespace state {
	namespace detail {
		template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer;

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneInvalid>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneNewlyCreated);
			assert(false);
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneInvalid>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneNewlyCreated);
			assert(false);
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneNewlyCreated>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random, CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneNewlyCreated);
			return; // do nothing
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneNewlyCreated>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random, CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneNewlyCreated);
			return; // do nothing
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneRemoved);
			return; // do nothing
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneRemoved);
			return; // do nothing
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneSetASide);
			return; // do nothing
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneSetASide);
			return; // do nothing
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneDeck>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneDeck);
			board.Get(card.GetPlayerIdentifier()).deck_.ShuffleAdd(card_ref, [&random](auto exclusive_max) {
				return random.Get(exclusive_max);
			});
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneDeck>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneDeck);
			board.Get(card.GetPlayerIdentifier()).deck_.RemoveLast();
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneGraveyard);
			board.Get(card.GetPlayerIdentifier()).graveyard_.Add<TargetCardType>(card_ref);
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneGraveyard);
			board.Get(card.GetPlayerIdentifier()).graveyard_.Remove<TargetCardType>(card_ref);
		}

		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneHand);
			int pos = (int)board.Get(card.GetPlayerIdentifier()).hand_.PushBack(card_ref);
			card.SetZonePos()(pos);
		}
		template <CardType TargetCardType>
		inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == TargetCardType);
			assert(card.GetZone() == kCardZoneHand);

			int pos = card.GetZonePosition();
			assert(board.Get(card.GetPlayerIdentifier()).hand_.Get(pos) == card_ref);
			auto& hand = board.Get(card.GetPlayerIdentifier()).hand_;
			hand.Remove(pos, [&cards_mgr](CardRef ref, size_t pos) {
				cards_mgr.SetCardZonePos(ref, (int)pos);
			});
		}

		inline void PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeHero);
			assert(card.GetZone() == kCardZonePlay);
			board::Player & player = board.Get(card.GetPlayerIdentifier());
			if (player.GetHeroRef().IsValid()) throw std::exception("hero should be removed first");
			player.SetHeroRef(card_ref);
		}
		inline void PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeHero);
			assert(card.GetZone() == kCardZonePlay);
			board::Player & player = board.Get(card.GetPlayerIdentifier());
			player.GetHeroRef().Invalidate();
		}

		inline void PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card, int pos)
		{
			assert(card.GetCardType() == kCardTypeMinion);
			assert(card.GetZone() == kCardZonePlay);

			board.Get(card.GetPlayerIdentifier()).minions_.Insert(card_ref, pos, [&cards_mgr](CardRef ref, size_t pos)
			{
				cards_mgr.SetCardZonePos(ref, (int)pos);
			});
		}
		inline void PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeMinion);
			assert(card.GetZone() == kCardZonePlay);

			board.Get(card.GetPlayerIdentifier()).minions_.Remove(
				card.GetZonePosition(),
				[&cards_mgr](CardRef ref, size_t pos) {
				cards_mgr.SetCardZonePos(ref, (int)pos);
			});
		}

		inline void PlayerDataStructureMaintainer<kCardTypeSecret, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeSecret);
			assert(card.GetZone() == kCardZonePlay);
			board::Player & player = board.Get(card.GetPlayerIdentifier());
			return player.secrets_.Add(card.GetCardId(), card_ref);
		}
		inline void PlayerDataStructureMaintainer<kCardTypeSecret, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeSecret);
			assert(card.GetZone() == kCardZonePlay);
			board::Player & player = board.Get(card.GetPlayerIdentifier());
			return player.secrets_.Remove(card.GetCardId());
		}

		inline void PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeWeapon);
			assert(card.GetZone() == kCardZonePlay);
			board::Player & player = board.Get(card.GetPlayerIdentifier());
			assert(cards_mgr.Get(player.GetHeroRef()).GetRawData().weapon_ref == card_ref);
		}
		inline void PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeWeapon);
			assert(card.GetZone() == kCardZonePlay);
			board::Player & player = board.Get(card.GetPlayerIdentifier());
			assert(!cards_mgr.Get(player.GetHeroRef()).GetRawData().weapon_ref.IsValid());
		}

		inline void PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeSpell);
			assert(card.GetZone() == kCardZonePlay);
			// do nothing
		}
		inline void PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeSpell);
			assert(card.GetZone() == kCardZonePlay);
			// do nothing
		}

		inline void PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeHeroPower);
			assert(card.GetZone() == kCardZonePlay);
			board::Player & player = board.Get(card.GetPlayerIdentifier());
			assert(player.hero_power_ref_.IsValid() == false);
			player.SetHeroPowerRef(card_ref);
		}
		inline void PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeHeroPower);
			assert(card.GetZone() == kCardZonePlay);
			board::Player & player = board.Get(card.GetPlayerIdentifier());
			player.InvalidateHeroPowerRef();
		}

		inline void PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay>::
			Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeEnchantment);
			assert(card.GetZone() == kCardZonePlay);
			// do nothing
		}
		inline void PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay>::
			Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card)
		{
			assert(card.GetCardType() == kCardTypeEnchantment);
			assert(card.GetZone() == kCardZonePlay);
			// do nothing
		}
	}
}