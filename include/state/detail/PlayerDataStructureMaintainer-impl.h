#pragma once

#include <assert.h>
#include "state/ZoneChanger.h"

namespace state {
	template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer;

	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneInvalid>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneInvalid);
		return; // do nothing
	}
	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneInvalid>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneInvalid);
		return; // do nothing
	}

	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneRemoved);
		return; // do nothing
	}
	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneRemoved);
		return; // do nothing
	}

	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneSetASide);
		return; // do nothing
	}
	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneSetASide);
		return; // do nothing
	}

	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneDeck>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneDeck);
		state.GetBoard().Get(card.GetPlayerIdentifier()).deck_.ShuffleAdd(card_ref, [flow_context](auto exclusive_max) {
			return flow_context.random_.Get(exclusive_max);
		});
	}
	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneDeck>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneDeck);
		state.GetBoard().Get(card.GetPlayerIdentifier()).deck_.RemoveLast();
	}

	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneGraveyard);
		state.GetBoard().Get(card.GetPlayerIdentifier()).graveyard_.Add<TargetCardType>(card_ref);
	}
	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneGraveyard);
		state.GetBoard().Get(card.GetPlayerIdentifier()).graveyard_.Remove<TargetCardType>(card_ref);
	}

	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneHand);
		int pos = (int)state.GetBoard().Get(card.GetPlayerIdentifier()).hand_.PushBack(card_ref);
		card.SetLocation().Position(pos);
	}
	template <CardType TargetCardType>
	inline void PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == TargetCardType);
		assert(card.GetZone() == kCardZoneHand);

		int pos = card.GetZonePosition();
		assert(state.GetBoard().Get(card.GetPlayerIdentifier()).hand_.Get(pos) == card_ref);
		auto& hand = state.GetBoard().Get(card.GetPlayerIdentifier()).hand_;
		hand.Remove(pos, [&state, &flow_context](CardRef ref, size_t pos) {
			state.SetCardZonePos(ref, (int)pos);
		});
	}

	inline void PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == kCardTypeHero);
		assert(card.GetZone() == kCardZonePlay);
		board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
		if (player.hero_ref_.IsValid()) throw std::exception("hero should be removed first");
		player.hero_ref_ = card_ref;
	}
	inline void PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == kCardTypeHero);
		assert(card.GetZone() == kCardZonePlay);
		board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
		player.hero_ref_.Invalidate();
	}

	inline void PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card, int pos)
	{
		assert(card.GetCardType() == kCardTypeMinion);
		assert(card.GetZone() == kCardZonePlay);

		state.GetBoard().Get(card.GetPlayerIdentifier()).minions_.Insert(card_ref, pos, [&state, &flow_context](CardRef ref, size_t pos) {
			state.SetCardZonePos(ref, (int)pos);
		});
	}
	inline void PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == kCardTypeMinion);
		assert(card.GetZone() == kCardZonePlay);

		state.GetBoard().Get(card.GetPlayerIdentifier()).minions_.Remove(
			card.GetZonePosition(),
			[&state, &flow_context](CardRef ref, size_t pos) {
			state.SetCardZonePos(ref, (int)pos);
		});
	}

	inline void PlayerDataStructureMaintainer<kCardTypeSecret, kCardZonePlay>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == kCardTypeSecret);
		assert(card.GetZone() == kCardZonePlay);
		board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
		return player.secrets_.Add(card.GetCardId(), card_ref);
	}
	inline void PlayerDataStructureMaintainer<kCardTypeSecret, kCardZonePlay>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == kCardTypeSecret);
		assert(card.GetZone() == kCardZonePlay);
		board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
		return player.secrets_.Remove(card.GetCardId());
	}

	inline void PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == kCardTypeWeapon);
		assert(card.GetZone() == kCardZonePlay);
		board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
		assert(state.GetCardsManager().Get(player.hero_ref_).GetRawData().weapon_ref == card_ref);
	}
	inline void PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == kCardTypeWeapon);
		assert(card.GetZone() == kCardZonePlay);
		board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
		assert(!state.GetCardsManager().Get(player.hero_ref_).GetRawData().weapon_ref.IsValid());
	}

	inline void PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == kCardTypeSpell);
		assert(card.GetZone() == kCardZonePlay);
		// do nothing
	}
	inline void PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == kCardTypeSpell);
		assert(card.GetZone() == kCardZonePlay);
		// do nothing
	}

	inline void PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == kCardTypeHeroPower);
		assert(card.GetZone() == kCardZonePlay);
		// do nothing
	}
	inline void PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == kCardTypeHeroPower);
		assert(card.GetZone() == kCardZonePlay);
		// do nothing
	}

	inline void PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay>::
		Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card)
	{
		assert(card.GetCardType() == kCardTypeEnchantment);
		assert(card.GetZone() == kCardZonePlay);
		// do nothing
	}
	inline void PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay>::
		Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card) {
		assert(card.GetCardType() == kCardTypeEnchantment);
		assert(card.GetZone() == kCardZonePlay);
		// do nothing
	}
}