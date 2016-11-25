#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "Utils/StaticEventTriggerer.h"

namespace state
{
	namespace Manipulators
	{
		namespace Events
		{
			namespace impl
			{
				namespace RemovedFromZone
				{
					template <CardType RemovingCardType, CardZone RemovingCardZone>
					class RemoveFromPlayerDatStructure
					{
					public:
						static void Trigger(State & state, CardRef card_ref, Cards::Card & card)
						{
							switch (RemovingCardZone)
							{
							case kCardZoneDeck:
								return RemoveFromDeckZone(state, card_ref, card);
							case kCardZoneHand:
								return RemoveFromHandZone(state, card_ref, card);
							case kCardZonePlay:
								return RemoveFromPlayZone(state, card_ref, card);
							case kCardZoneGraveyard:
								return RemoveFromGraveyardZone(state, card_ref, card);
								break;
							}
						}

					private:
						static void RemoveFromDeckZone(State & state, CardRef card_ref, Cards::Card & card)
						{
							Player & player = state.board.players.Get(card.GetPlayerIdentifier());
							player.deck_.GetLocationManipulator().Remove(state, card.GetZonePosition());
						}

						static void RemoveFromHandZone(State & state, CardRef card_ref, Cards::Card & card)
						{
							Player & player = state.board.players.Get(card.GetPlayerIdentifier());
							player.hand_.GetLocationManipulator().Remove(state, card.GetZonePosition());
						}

						static void RemoveFromPlayZone(State & state, CardRef card_ref, Cards::Card & card)
						{
							Player & player = state.board.players.Get(card.GetPlayerIdentifier());

							switch (RemovingCardType)
							{
							case kCardTypeMinion:
								return player.minions_.GetLocationManipulator().Remove(state, card.GetZonePosition());
							case kCardTypeWeapon:
								return player.weapon_.Destroy();
							case kCardTypeSecret:
								return player.secrets_.Remove(card.GetCardId());
							}
						}

						static void RemoveFromGraveyardZone(State & state, CardRef card_ref, Cards::Card & card)
						{
							Player & player = state.board.players.Get(card.GetPlayerIdentifier());
							player.graveyard_.GetLocationManipulator<RemovingCardType>().Remove(state, card.GetZonePosition());
						}
					};
				}
			}

			template <CardType RemovingCardType, CardZone RemovingCardZone>
			using RemovedFromZoneEvent = ::Utils::StaticEventTriggerer <
				impl::RemovedFromZone::RemoveFromPlayerDatStructure<RemovingCardType, RemovingCardZone>
			>;
		}
	}
}