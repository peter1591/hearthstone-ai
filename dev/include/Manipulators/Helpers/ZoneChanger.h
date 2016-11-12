#pragma once

#include "Entity/Card.h"
#include "Entity/CardType.h"
#include "Entity/CardZone.h"
#include "EntitiesManager/CardRef.h"
#include "State/State.h"
#include "State/PlayerIdentifier.h"
#include "State/Player.h"

namespace Manipulators
{
	namespace Helpers
	{
		template <Entity::CardType ChangingCardType>
		class ZoneChanger
		{
		public:
			ZoneChanger(EntitiesManager& mgr, CardRef card_ref, Entity::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

			void Change(State::State & state, State::PlayerIdentifier player_identifier, Entity::CardZone zone, int pos)
			{
				Remove(state);

				auto location_setter = card_.GetLocationSetter();
				location_setter.SetPlayerIdentifier(player_identifier);
				location_setter.SetZone(zone);
				location_setter.SetZonePosition(pos);

				Add(state);
			}

			void Add(State::State & state)
			{
				switch (card_.GetZone())
				{
				case Entity::kCardZoneDeck:
					return AddToDeckZone(state);
				case Entity::kCardZoneHand:
					return AddToHandZone(state);
				case Entity::kCardZonePlay:
					return AddToPlayZone(state);
				case Entity::kCardZoneGraveyard:
					return AddToGraveyardZone(state);
				}
			}

		private:
			void Remove(State::State & state)
			{
				switch (card_.GetZone())
				{
				case Entity::kCardZoneDeck:
					return RemoveFromDeckZone(state);
				case Entity::kCardZoneHand:
					return RemoveFromHandZone(state);
				case Entity::kCardZonePlay:
					return RemoveFromPlayZone(state);
				case Entity::kCardZoneGraveyard:
					return RemoveFromGraveyardZone(state);
				}
			}

			void AddToDeckZone(State::State & state)
			{
				State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
				player.deck_.GetLocationManipulator().Insert(mgr_, card_ref_);
			}
			void RemoveFromDeckZone(State::State & state)
			{
				State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
				player.deck_.GetLocationManipulator().Remove(mgr_, card_.GetZonePosition());
			}

			void AddToHandZone(State::State & state)
			{
				State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
				player.hand_.GetLocationManipulator().Insert(mgr_, card_ref_);
			}
			void RemoveFromHandZone(State::State & state)
			{
				State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
				player.hand_.GetLocationManipulator().Remove(mgr_, card_.GetZonePosition());
			}

			void AddToPlayZone(State::State & state);
			void RemoveFromPlayZone(State::State & state);

			void AddToGraveyardZone(State::State & state)
			{
				State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
				player.graveyard_.GetLocationManipulator<ChangingCardType>().Insert(mgr_, card_ref_);
			}
			void RemoveFromGraveyardZone(State::State & state)
			{
				State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
				player.graveyard_.GetLocationManipulator<ChangingCardType>().Remove(mgr_, card_.GetZonePosition());
			}

		private:
			EntitiesManager & mgr_;
			CardRef card_ref_;
			Entity::Card & card_;
		};

		template <>
		void ZoneChanger<Entity::kCardTypeMinion>::AddToPlayZone(State::State & state)
		{
			State::Player & player = state.players.Get(card_.GetPlayerIdentifier());

			player.minions_.GetLocationManipulator().Insert(mgr_, card_ref_);
		}
		template <>
		void ZoneChanger<Entity::kCardTypeMinion>::RemoveFromPlayZone(State::State & state)
		{
			State::Player & player = state.players.Get(card_.GetPlayerIdentifier());

			player.minions_.GetLocationManipulator().Remove(mgr_, card_.GetZonePosition());
		}

		template <>
		void ZoneChanger<Entity::kCardTypeWeapon>::AddToPlayZone(State::State & state)
		{
			State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
			player.weapon_.Equip(card_ref_);
		}
		template <>
		void ZoneChanger<Entity::kCardTypeWeapon>::RemoveFromPlayZone(State::State & state)
		{
			State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
			player.weapon_.Destroy();
		}

		template <>
		void ZoneChanger<Entity::kCardTypeSecret>::AddToPlayZone(State::State & state)
		{
			State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
			player.secrets_.Add(card_.GetCardId(), card_ref_);
		}
		template <>
		void ZoneChanger<Entity::kCardTypeSecret>::RemoveFromPlayZone(State::State & state)
		{
			State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
			player.secrets_.Remove(card_.GetCardId());
		}
	}
}