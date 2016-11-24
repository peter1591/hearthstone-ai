#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "StaticEventManager/Triggerer.h"

namespace State
{
	namespace Manipulators
	{
		namespace Events
		{
			namespace impl
			{
				namespace AddToZone
				{
					template <CardType TargetCardType, CardZone TargetCardZone>
					class AddToPlayerDatStructure
					{
					public:
						static void Trigger(State & state, CardRef card_ref, Cards::Card & card)
						{
							switch (card.GetZone())
							{
							case kCardZoneDeck:
								return AddToDeckZone(state, card_ref, card);
							case kCardZoneHand:
								return AddToHandZone(state, card_ref, card);
							case kCardZonePlay:
								return AddToPlayZone(state, card_ref, card);
							case kCardZoneGraveyard:
								return AddToGraveyardZone(state, card_ref, card);
							}
						}

					private:
						static void AddToDeckZone(State & state, CardRef card_ref, Cards::Card & card)
						{
							Player & player = state.board.players.Get(card.GetPlayerIdentifier());
							player.deck_.GetLocationManipulator().Insert(state, card_ref);
						}

						static void AddToHandZone(State & state, CardRef card_ref, Cards::Card & card)
						{
							Player & player = state.board.players.Get(card.GetPlayerIdentifier());
							player.hand_.GetLocationManipulator().Insert(state, card_ref);
						}

						static void AddToPlayZone(State & state, CardRef card_ref, Cards::Card & card)
						{
							Player & player = state.board.players.Get(card.GetPlayerIdentifier());

							switch (TargetCardType)
							{
							case kCardTypeHero:
								if (player.hero_ref_.IsValid()) throw std::exception("hero should be removed first");
								player.hero_ref_ = card_ref;
								return;
							case kCardTypeMinion:
								return player.minions_.GetLocationManipulator().Insert(state, card_ref);
							case kCardTypeWeapon:
								return player.weapon_.Equip(card_ref);
							case kCardTypeSecret:
								return player.secrets_.Add(card.GetCardId(), card_ref);
							}
						}

						static void AddToGraveyardZone(State & state, CardRef card_ref, Cards::Card & card)
						{
							Player & player = state.board.players.Get(card.GetPlayerIdentifier());
							player.graveyard_.GetLocationManipulator<TargetCardType>().Insert(state, card_ref);
						}
					};
				}
			}

			template <CardType TargetCardType, CardZone TargetCardZone>
			using AddToZoneEvent = StaticEventManager::Triggerer <
				impl::AddToZone::AddToPlayerDatStructure<TargetCardType, TargetCardZone>
			>;
		}
	}
}