#pragma once

#include <assert.h>
#include "FlowControl/Manipulators/Helpers/ZoneChanger.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			template <state::CardType TargetCardType, state::CardZone TargetCardZone> struct PlayerDataStructureMaintainer;

			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneInvalid>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneInvalid);
				return; // do nothing
			}
			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneInvalid>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneInvalid);
				return; // do nothing
			}

			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneRemoved>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneRemoved);
				return; // do nothing
			}
			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneRemoved>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneRemoved);
				return; // do nothing
			}

			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneSetASide>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneSetASide);
				return; // do nothing
			}
			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneSetASide>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneSetASide);
				return; // do nothing
			}

			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneDeck>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneDeck);
				state.GetBoard().Get(card.GetPlayerIdentifier()).deck_.ShuffleAdd(card_ref, [flow_context](auto exclusive_max) {
					return flow_context.random_.Get(exclusive_max);
				});
			}
			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneDeck>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneDeck);
				state.GetBoard().Get(card.GetPlayerIdentifier()).deck_.RemoveLast();
			}

			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneGraveyard>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneGraveyard);
				state.GetBoard().Get(card.GetPlayerIdentifier()).graveyard_.Add<TargetCardType>(card_ref);
			}
			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneGraveyard>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneGraveyard);
				state.GetBoard().Get(card.GetPlayerIdentifier()).graveyard_.Remove<TargetCardType>(card_ref);
			}

			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneHand>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneHand);
				int pos = (int)state.GetBoard().Get(card.GetPlayerIdentifier()).hand_.PushBack(card_ref);
				card.SetLocation().Position(pos);
			}
			template <state::CardType TargetCardType>
			inline void PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneHand>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == TargetCardType);
				assert(card.GetZone() == state::kCardZoneHand);

				int pos = card.GetZonePosition();
				assert(state.GetBoard().Get(card.GetPlayerIdentifier()).hand_.Get(pos) == card_ref);
				auto& hand = state.GetBoard().Get(card.GetPlayerIdentifier()).hand_;
				hand.Remove(pos, [&state, &flow_context](state::CardRef ref, size_t pos) {
					Manipulate(state, flow_context).Card(ref).ZonePosition().Set((int)pos);
				});
			}

			inline void PlayerDataStructureMaintainer<state::kCardTypeHero, state::kCardZonePlay>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == state::kCardTypeHero);
				assert(card.GetZone() == state::kCardZonePlay);
				state::board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
				if (player.hero_ref_.IsValid()) throw std::exception("hero should be removed first");
				player.hero_ref_ = card_ref;
			}
			inline void PlayerDataStructureMaintainer<state::kCardTypeHero, state::kCardZonePlay>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == state::kCardTypeHero);
				assert(card.GetZone() == state::kCardZonePlay);
				state::board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
				player.hero_ref_.Invalidate();
			}

			inline void PlayerDataStructureMaintainer<state::kCardTypeMinion, state::kCardZonePlay>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, int pos)
			{
				assert(card.GetCardType() == state::kCardTypeMinion);
				assert(card.GetZone() == state::kCardZonePlay);

				state.GetBoard().Get(card.GetPlayerIdentifier()).minions_.Insert(card_ref, pos, [&state, &flow_context](state::CardRef ref, size_t pos) {
					Manipulate(state, flow_context).Card(ref).ZonePosition().Set((int)pos);
				});
			}
			inline void PlayerDataStructureMaintainer<state::kCardTypeMinion, state::kCardZonePlay>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == state::kCardTypeMinion);
				assert(card.GetZone() == state::kCardZonePlay);

				state.GetBoard().Get(card.GetPlayerIdentifier()).minions_.Remove(
					card.GetZonePosition(),
					[&state, &flow_context](state::CardRef ref, size_t pos) {
						Manipulate(state, flow_context).Card(ref).ZonePosition().Set((int)pos);
				});
			}

			inline void PlayerDataStructureMaintainer<state::kCardTypeSecret, state::kCardZonePlay>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == state::kCardTypeSecret);
				assert(card.GetZone() == state::kCardZonePlay);
				state::board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
				return player.secrets_.Add(card.GetCardId(), card_ref);
			}
			inline void PlayerDataStructureMaintainer<state::kCardTypeSecret, state::kCardZonePlay>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == state::kCardTypeSecret);
				assert(card.GetZone() == state::kCardZonePlay);
				state::board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
				return player.secrets_.Remove(card.GetCardId());
			}

			inline void PlayerDataStructureMaintainer<state::kCardTypeWeapon, state::kCardZonePlay>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == state::kCardTypeWeapon);
				assert(card.GetZone() == state::kCardZonePlay);
				state::board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
				assert(state.GetCardsManager().Get(player.hero_ref_).GetRawData().weapon_ref == card_ref);
			}
			inline void PlayerDataStructureMaintainer<state::kCardTypeWeapon, state::kCardZonePlay>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == state::kCardTypeWeapon);
				assert(card.GetZone() == state::kCardZonePlay);
				state::board::Player & player = state.GetBoard().Get(card.GetPlayerIdentifier());
				assert(!state.GetCardsManager().Get(player.hero_ref_).GetRawData().weapon_ref.IsValid());
			}

			inline void PlayerDataStructureMaintainer<state::kCardTypeSpell, state::kCardZonePlay>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == state::kCardTypeSpell);
				assert(card.GetZone() == state::kCardZonePlay);
				// do nothing
			}
			inline void PlayerDataStructureMaintainer<state::kCardTypeSpell, state::kCardZonePlay>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == state::kCardTypeSpell);
				assert(card.GetZone() == state::kCardZonePlay);
				// do nothing
			}

			inline void PlayerDataStructureMaintainer<state::kCardTypeHeroPower, state::kCardZonePlay>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == state::kCardTypeHeroPower);
				assert(card.GetZone() == state::kCardZonePlay);
				// do nothing
			}
			inline void PlayerDataStructureMaintainer<state::kCardTypeHeroPower, state::kCardZonePlay>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == state::kCardTypeHeroPower);
				assert(card.GetZone() == state::kCardZonePlay);
				// do nothing
			}

			inline void PlayerDataStructureMaintainer<state::kCardTypeEnchantment, state::kCardZonePlay>::
				Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card)
			{
				assert(card.GetCardType() == state::kCardTypeEnchantment);
				assert(card.GetZone() == state::kCardZonePlay);
				// do nothing
			}
			inline void PlayerDataStructureMaintainer<state::kCardTypeEnchantment, state::kCardZonePlay>::
				Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				assert(card.GetCardType() == state::kCardTypeEnchantment);
				assert(card.GetZone() == state::kCardZonePlay);
				// do nothing
			}
		}
	}
}