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
		private:
			template <Entity::CardZone ChangeToZone> int GetNewlyAddedZonePosition(const State::State & state);
			template <Entity::CardZone ChangeToZone> struct NewlyAddedHasDefaultZonePosition
			{
				static constexpr bool value = false;
			};

		public:
			ZoneChanger(EntitiesManager& mgr, CardRef card_ref, Entity::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

			template <Entity::CardZone ChangeToZone,
				typename std::enable_if_t<NewlyAddedHasDefaultZonePosition<ChangeToZone>::value, nullptr_t> = nullptr>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier)
			{
				int new_pos = GetNewlyAddedZonePosition<ChangeToZone>(state);
				return ChangeToInternal<ChangeToZone>(state, player_identifier, new_pos);
			}

			template <Entity::CardZone ChangeToZone,
				typename std::enable_if_t<!NewlyAddedHasDefaultZonePosition<ChangeToZone>::value, nullptr_t> = nullptr>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier, int pos)
			{
				return ChangeToInternal<ChangeToZone>(state, player_identifier, pos);
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
			template <>
			struct NewlyAddedHasDefaultZonePosition<Entity::kCardZoneDeck>
			{
				static constexpr bool value = true;
			};

			template <>
			int GetNewlyAddedZonePosition<Entity::kCardZoneDeck>(const State::State & state)
			{
				const State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
				return player.deck_.Size();
			}

			template <>
			struct NewlyAddedHasDefaultZonePosition<Entity::kCardZoneHand>
			{
				static constexpr bool value = true;
			};

			template <>
			int GetNewlyAddedZonePosition<Entity::kCardZoneHand>(const State::State & state)
			{
				const State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
				return player.hand_.Size();
			}

			template <>
			struct NewlyAddedHasDefaultZonePosition<Entity::kCardZoneGraveyard>
			{
				static constexpr bool value = true;
			};

			template <>
			int GetNewlyAddedZonePosition<Entity::kCardZoneGraveyard>(const State::State & state)
			{
				const State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
				return player.graveyard_.GetTotalOthers();
			}

			template <Entity::CardZone ChangeToZone>
			void ChangeToInternal(State::State & state, State::PlayerIdentifier player_identifier, int pos)
			{
				Remove(state);

				auto location_setter = card_.GetLocationSetter();
				location_setter.SetPlayerIdentifier(player_identifier);
				location_setter.SetZone(ChangeToZone);
				location_setter.SetZonePosition(pos);

				Add(state);
			}

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
		template <>
		int ZoneChanger<Entity::kCardTypeMinion>::GetNewlyAddedZonePosition<Entity::kCardZoneGraveyard>(const State::State & state)
		{
			const State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
			return player.graveyard_.GetTotalMinions();
		}
		template <>
		template <>
		int ZoneChanger<Entity::kCardTypeSpell>::GetNewlyAddedZonePosition<Entity::kCardZoneGraveyard>(const State::State & state)
		{
			const State::Player & player = state.players.Get(card_.GetPlayerIdentifier());
			return player.graveyard_.GetTotalSpells();
		}

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