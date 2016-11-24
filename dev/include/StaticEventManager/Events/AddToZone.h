#pragma once

#include <iostream>

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
			namespace AddToZone
			{
				template <State::CardType TargetCardType, State::CardZone TargetCardZone>
				class AddToPlayerDatStructure
				{
				public:
					static void Trigger(State::State & state, CardRef card_ref, State::Cards::Card & card)
					{
						switch (card.GetZone())
						{
						case State::kCardZoneDeck:
							return AddToDeckZone(state, card_ref, card);
						case State::kCardZoneHand:
							return AddToHandZone(state, card_ref, card);
						case State::kCardZonePlay:
							return AddToPlayZone(state, card_ref, card);
						case State::kCardZoneGraveyard:
							return AddToGraveyardZone(state, card_ref, card);
						}
					}

				private:
					static void AddToDeckZone(State::State & state, CardRef card_ref, State::Cards::Card & card)
					{
						State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());
						player.deck_.GetLocationManipulator().Insert(state, card_ref);
					}

					static void AddToHandZone(State::State & state, CardRef card_ref, State::Cards::Card & card)
					{
						State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());
						player.hand_.GetLocationManipulator().Insert(state, card_ref);
					}

					static void AddToPlayZone(State::State & state, CardRef card_ref, State::Cards::Card & card)
					{
						State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());

						switch (TargetCardType)
						{
						case State::kCardTypeHero:
							if (player.hero_ref_.IsValid()) throw std::exception("hero should be removed first");
							player.hero_ref_ = card_ref;
							return;
						case State::kCardTypeMinion:
							return player.minions_.GetLocationManipulator().Insert(state, card_ref);
						case State::kCardTypeWeapon:
							return player.weapon_.Equip(card_ref);
						case State::kCardTypeSecret:
							return player.secrets_.Add(card.GetCardId(), card_ref);
						}
					}

					static void AddToGraveyardZone(State::State & state, CardRef card_ref, State::Cards::Card & card)
					{
						State::Player & player = state.board.players.Get(card.GetPlayerIdentifier());
						player.graveyard_.GetLocationManipulator<TargetCardType>().Insert(state, card_ref);
					}
				};
			}
		}

		template <State::CardType TargetCardType, State::CardZone TargetCardZone>
		using AddToZoneEvent = Triggerer <
			impl::AddToZone::AddToPlayerDatStructure<TargetCardType, TargetCardZone>
			>;
	}
}