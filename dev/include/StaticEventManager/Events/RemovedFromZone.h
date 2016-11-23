#pragma once

#include <iostream>

#include "StaticEventManager/Triggerer.h"
#include "State/State.h"
#include "EntitiesManager/CardRef.h"
#include "Entity/Card.h"
#include "Entity/CardType.h"
#include "Entity/CardZone.h"

namespace StaticEventManager
{
	namespace Events
	{
		namespace impl
		{
			namespace RemovedFromZone
			{
				template <Entity::CardType RemovingCardType, Entity::CardZone RemovingCardZone>
				class RemoveFromPlayerDatStructure
				{
				public:
					static void Trigger(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						std::cout << "Removed from zone called. "
							<< "Type = " << RemovingCardType
							<< ", Zone = " << RemovingCardZone
							<< std::endl;

						switch (RemovingCardZone)
						{
						case Entity::kCardZoneDeck:
							return RemoveFromDeckZone(state, card_ref, card);
						case Entity::kCardZoneHand:
							return RemoveFromHandZone(state, card_ref, card);
						case Entity::kCardZonePlay:
							return RemoveFromPlayZone(state, card_ref, card);
						case Entity::kCardZoneGraveyard:
							return RemoveFromGraveyardZone(state, card_ref, card);
							break;
						}
					}

				private:
					static void RemoveFromDeckZone(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						State::Player & player = state.players.Get(card.GetPlayerIdentifier());
						player.deck_.GetLocationManipulator().Remove(state.mgr, card.GetZonePosition());
					}

					static void RemoveFromHandZone(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						State::Player & player = state.players.Get(card.GetPlayerIdentifier());
						player.hand_.GetLocationManipulator().Remove(state.mgr, card.GetZonePosition());
					}

					static void RemoveFromPlayZone(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						State::Player & player = state.players.Get(card.GetPlayerIdentifier());

						switch (RemovingCardType)
						{
						case Entity::kCardTypeMinion:
							return player.minions_.GetLocationManipulator().Remove(state.mgr, card.GetZonePosition());
						case Entity::kCardTypeWeapon:
							return player.weapon_.Destroy();
						case Entity::kCardTypeSecret:
							return player.secrets_.Remove(card.GetCardId());
						}
					}

					static void RemoveFromGraveyardZone(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						State::Player & player = state.players.Get(card.GetPlayerIdentifier());
						player.graveyard_.GetLocationManipulator<RemovingCardType>().Remove(state.mgr, card.GetZonePosition());
					}
				};
			}
		}

		template <Entity::CardType RemovingCardType, Entity::CardZone RemovingCardZone>
		using RemovedFromZoneEvent = Triggerer <
			impl::RemovedFromZone::RemoveFromPlayerDatStructure<RemovingCardType, RemovingCardZone>
			>;
	}
}