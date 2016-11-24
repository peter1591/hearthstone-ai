#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "StaticEventManager/Triggerer.h"

namespace StaticEventManager
{
	namespace Events
	{
		namespace impl
		{
			namespace RemovedFromZone
			{
				template <State::CardType RemovingCardType, State::CardZone RemovingCardZone>
				class RemoveFromPlayerDatStructure
				{
				public:
					static void Trigger(::State::State & state, CardRef card_ref, ::State::Cards::Card & card)
					{
						switch (RemovingCardZone)
						{
						case ::State::kCardZoneDeck:
							return RemoveFromDeckZone(state, card_ref, card);
						case ::State::kCardZoneHand:
							return RemoveFromHandZone(state, card_ref, card);
						case ::State::kCardZonePlay:
							return RemoveFromPlayZone(state, card_ref, card);
						case ::State::kCardZoneGraveyard:
							return RemoveFromGraveyardZone(state, card_ref, card);
							break;
						}
					}

				private:
					static void RemoveFromDeckZone(::State::State & state, CardRef card_ref, ::State::Cards::Card & card)
					{
						::State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());
						player.deck_.GetLocationManipulator().Remove(state, card.GetZonePosition());
					}

					static void RemoveFromHandZone(::State::State & state, CardRef card_ref, ::State::Cards::Card & card)
					{
						::State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());
						player.hand_.GetLocationManipulator().Remove(state, card.GetZonePosition());
					}

					static void RemoveFromPlayZone(::State::State & state, CardRef card_ref, ::State::Cards::Card & card)
					{
						::State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());

						switch (RemovingCardType)
						{
						case ::State::kCardTypeMinion:
							return player.minions_.GetLocationManipulator().Remove(state, card.GetZonePosition());
						case ::State::kCardTypeWeapon:
							return player.weapon_.Destroy();
						case ::State::kCardTypeSecret:
							return player.secrets_.Remove(card.GetCardId());
						}
					}

					static void RemoveFromGraveyardZone(::State::State & state, CardRef card_ref, ::State::Cards::Card & card)
					{
						::State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());
						player.graveyard_.GetLocationManipulator<RemovingCardType>().Remove(state, card.GetZonePosition());
					}
				};
			}
		}

		template <State::CardType RemovingCardType, State::CardZone RemovingCardZone>
		using RemovedFromZoneEvent = Triggerer <
			impl::RemovedFromZone::RemoveFromPlayerDatStructure<RemovingCardType, RemovingCardZone>
			>;
	}
}