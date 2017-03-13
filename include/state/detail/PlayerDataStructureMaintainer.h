#pragma once

#include <assert.h>
#include "state/Types.h"
#include "state/Cards/Card.h"

namespace state {
	template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer;

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneInvalid> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneDeck>
	{
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>
	{
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>
	{
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <>
	struct PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = true;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card, int pos);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeSecret, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
}