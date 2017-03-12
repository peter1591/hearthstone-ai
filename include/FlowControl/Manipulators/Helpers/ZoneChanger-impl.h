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
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneRemoved> {
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetZone() == state::kCardZoneRemoved);
					return; // do nothing
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetZone() == state::kCardZoneRemoved);
					return; // do nothing
				}
			};

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneSetASide> {
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetZone() == state::kCardZoneSetASide);
					return; // do nothing
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetZone() == state::kCardZoneSetASide);
					return; // do nothing
				}
			};

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneDeck>
			{
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetZone() == state::kCardZoneDeck);
					state.board.Get(card.GetPlayerIdentifier()).deck_.ShuffleAdd(card_ref, [flow_context](auto exclusive_max) {
						return flow_context.random_.Get(exclusive_max);
					});
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetZone() == state::kCardZoneDeck);
					state.board.Get(card.GetPlayerIdentifier()).deck_.RemoveLast();
				}
			};

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneGraveyard>
			{
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetZone() == state::kCardZoneGraveyard);
					state.board.Get(card.GetPlayerIdentifier()).graveyard_.Add<TargetCardType>(card_ref);
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetZone() == state::kCardZoneGraveyard);
					state.board.Get(card.GetPlayerIdentifier()).graveyard_.Remove<TargetCardType>(card_ref);
				}
			};

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneHand>
			{
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == TargetCardType);
					assert(card.GetZone() == state::kCardZoneHand);
					OrderedCardsManager::FromHand(state, flow_context, card.GetPlayerIdentifier()).Insert(card_ref);
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == TargetCardType);
					assert(card.GetZone() == state::kCardZoneHand);
					OrderedCardsManager::FromHand(state, flow_context, card.GetPlayerIdentifier()).Remove(card.GetZonePosition());
				}
			};

			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeHero, state::kCardZonePlay> {
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeHero);
					assert(card.GetZone() == state::kCardZonePlay);
					state::board::Player & player = state.board.Get(card.GetPlayerIdentifier());
					if (player.hero_ref_.IsValid()) throw std::exception("hero should be removed first");
					player.hero_ref_ = card_ref;
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeHero);
					assert(card.GetZone() == state::kCardZonePlay);
					state::board::Player & player = state.board.Get(card.GetPlayerIdentifier());
					player.hero_ref_.Invalidate();
				}
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeMinion, state::kCardZonePlay> {
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeMinion);
					assert(card.GetZone() == state::kCardZonePlay);
					return OrderedCardsManager::FromMinions(state, flow_context, card.GetPlayerIdentifier()).Insert(card_ref);
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeMinion);
					assert(card.GetZone() == state::kCardZonePlay);
					return OrderedCardsManager::FromMinions(state, flow_context, card.GetPlayerIdentifier()).Remove(card.GetZonePosition());
				}
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeSecret, state::kCardZonePlay> {
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeSecret);
					assert(card.GetZone() == state::kCardZonePlay);
					state::board::Player & player = state.board.Get(card.GetPlayerIdentifier());
					return player.secrets_.Add(card.GetCardId(), card_ref);
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeSecret);
					assert(card.GetZone() == state::kCardZonePlay);
					state::board::Player & player = state.board.Get(card.GetPlayerIdentifier());
					return player.secrets_.Remove(card.GetCardId());
				}
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeWeapon, state::kCardZonePlay> {
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeWeapon);
					assert(card.GetZone() == state::kCardZonePlay);
					state::board::Player & player = state.board.Get(card.GetPlayerIdentifier());
					assert(state.mgr.Get(player.hero_ref_).GetRawData().weapon_ref == card_ref);
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeWeapon);
					assert(card.GetZone() == state::kCardZonePlay);
					state::board::Player & player = state.board.Get(card.GetPlayerIdentifier());
					assert(!state.mgr.Get(player.hero_ref_).GetRawData().weapon_ref.IsValid());
				}
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeSpell, state::kCardZonePlay> {
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeSpell);
					assert(card.GetZone() == state::kCardZonePlay);
					// do nothing
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeSpell);
					assert(card.GetZone() == state::kCardZonePlay);
					// do nothing
				}
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeHeroPower, state::kCardZonePlay> {
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeHeroPower);
					assert(card.GetZone() == state::kCardZonePlay);
					// do nothing
				}
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
					assert(card.GetCardType() == state::kCardTypeHeroPower);
					assert(card.GetZone() == state::kCardZonePlay);
					// do nothing
				}
			};
		}
	}
}