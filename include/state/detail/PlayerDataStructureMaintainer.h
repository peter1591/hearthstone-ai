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
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate, CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate, CardRef card_ref);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneNewlyCreated> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>
		{
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
		};

		template <CardType TargetCardType>
		struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>
		{
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
		};

		template <>
		struct PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void ReplaceBy(board::Board & board, Cards::Manager & cards_mgr, CardRef card_ref, CardRef new_card_ref);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = true;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref, int pos);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void ReplaceBy(board::Board & board, Cards::Manager & cards_mgr, CardRef card_ref, CardRef new_card_ref);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void ReplaceBy(board::Board & board, Cards::Manager & cards_mgr, CardRef card_ref, CardRef new_card_ref);
		};
		template <>
		struct PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay> {
			static constexpr bool SpecifyAddPosition = false;
			static void Add(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
			static void Remove(board::Board & board, Cards::Manager & cards_mgr, FlowControl::Manipulate & manipulate,CardRef card_ref);
		};
	}
}