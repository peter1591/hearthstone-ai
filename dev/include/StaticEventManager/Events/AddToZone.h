#pragma once

#include <iostream>

#include "StaticEventManager/Triggerer.h"
#include "State/State.h"
#include "State/CardRef.h"
#include "State/Cards/Card.h"
#include "State/Types.h"

namespace StaticEventManager
{
	namespace Events
	{
		namespace impl
		{
			namespace AddToZone
			{
				template <Entity::CardType TargetCardType, Entity::CardZone TargetCardZone>
				class AddToPlayerDatStructure
				{
				public:
					static void Trigger(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						switch (card.GetZone())
						{
						case Entity::kCardZoneDeck:
							return AddToDeckZone(state, card_ref, card);
						case Entity::kCardZoneHand:
							return AddToHandZone(state, card_ref, card);
						case Entity::kCardZonePlay:
							return AddToPlayZone(state, card_ref, card);
						case Entity::kCardZoneGraveyard:
							return AddToGraveyardZone(state, card_ref, card);
						}
					}

				private:
					static void AddToDeckZone(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());
						player.deck_.GetLocationManipulator().Insert(state, card_ref);
					}

					static void AddToHandZone(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());
						player.hand_.GetLocationManipulator().Insert(state, card_ref);
					}

					static void AddToPlayZone(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());

						switch (TargetCardType)
						{
						case Entity::kCardTypeHero:
							if (player.hero_ref_.IsValid()) throw std::exception("hero should be removed first");
							player.hero_ref_ = card_ref;
							return;
						case Entity::kCardTypeMinion:
							return player.minions_.GetLocationManipulator().Insert(state, card_ref);
						case Entity::kCardTypeWeapon:
							return player.weapon_.Equip(card_ref);
						case Entity::kCardTypeSecret:
							return player.secrets_.Add(card.GetCardId(), card_ref);
						}
					}

					static void AddToGraveyardZone(State::State & state, CardRef card_ref, Entity::Card & card)
					{
						State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());
						player.graveyard_.GetLocationManipulator<TargetCardType>().Insert(state, card_ref);
					}
				};
			}
		}

		template <Entity::CardType TargetCardType, Entity::CardZone TargetCardZone>
		using AddToZoneEvent = Triggerer <
			impl::AddToZone::AddToPlayerDatStructure<TargetCardType, TargetCardZone>
			>;
	}
}