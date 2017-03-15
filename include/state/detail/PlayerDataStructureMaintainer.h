#pragma once

#include <assert.h>
#include "state/Types.h"
#include "state/Cards/Card.h"

namespace state {
	namespace board { class Board; }
	namespace Cards { class Manager; }

	namespace detail {
		template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer;

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneInvalid> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random, CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random, CardRef card_ref, Cards::Card & card);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneNewlyCreated> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneDeck>
		{
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>
		{
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>
		{
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};

		template <>
		struct PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = true;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card, int pos);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeSecret, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, IRandomGenerator & random,CardRef card_ref, Cards::Card & card);
		};
	}
}