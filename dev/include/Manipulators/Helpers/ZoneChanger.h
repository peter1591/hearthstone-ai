#pragma once

#include "Entity/Card.h"
#include "Entity/CardType.h"
#include "Entity/CardZone.h"
#include "EntitiesManager/CardRef.h"
#include "State/State.h"
#include "State/PlayerIdentifier.h"
#include "State/Board/Player.h"
#include "State/Utils/DefaultZonePosPolicy.h"
#include "StaticEventManager/Events/RemovedFromZone.h"
#include "StaticEventManager/Events/AddToZone.h"

namespace Manipulators
{
	namespace Helpers
	{
		template <Entity::CardZone ChangingCardZone, Entity::CardType ChangingCardType>
		class ZoneChanger
		{
		public:
			ZoneChanger(EntitiesManager& mgr, CardRef card_ref, Entity::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

			template <Entity::CardZone ChangeToZone,
				typename std::enable_if_t<State::Utils::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value, nullptr_t> = nullptr>
				void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier)
			{
				State::Player & player = state.board.players.Get(card_.GetPlayerIdentifier());
				int new_pos = State::Utils::DefaultZonePosGetter<ChangeToZone, ChangingCardType>()(player);
				return ChangeToInternal<ChangeToZone>(state, player_identifier, new_pos);
			}

			template <Entity::CardZone ChangeToZone,
				typename std::enable_if_t<!State::Utils::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value, nullptr_t> = nullptr>
				void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier, int pos)
			{
				return ChangeToInternal<ChangeToZone>(state, player_identifier, pos);
			}

			void Add(State::State & state)
			{
				StaticEventManager::Events::AddToZoneEvent<ChangingCardType, ChangingCardZone>
					::Trigger(state, card_ref_, card_);
			}

		private:
			template <Entity::CardZone ChangeToZone>
			void ChangeToInternal(State::State & state, State::PlayerIdentifier player_identifier, int pos)
			{
				StaticEventManager::Events::RemovedFromZoneEvent<ChangingCardType, ChangingCardZone>
					::Trigger(state, card_ref_, card_);

				card_.SetLocation()
					.Player(player_identifier)
					.Zone(ChangeToZone)
					.Position(pos);

				StaticEventManager::Events::AddToZoneEvent<ChangingCardType, ChangingCardZone>
					::Trigger(state, card_ref_, card_);
			}

		private:
			EntitiesManager & mgr_;
			CardRef card_ref_;
			Entity::Card & card_;
		};

		template <Entity::CardType ChangingCardType>
		class ZoneChangerWithUnknownZone
		{
		public:
			ZoneChangerWithUnknownZone(EntitiesManager& mgr, CardRef card_ref, Entity::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

			template <Entity::CardZone ChangeToZone>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier)
			{
				switch (card_.GetZone())
				{
				case Entity::kCardZoneDeck:
					return ZoneChanger<Entity::kCardZoneDeck, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardZoneGraveyard:
					return ZoneChanger<Entity::kCardZoneGraveyard, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardZoneHand:
					return ZoneChanger<Entity::kCardZoneHand, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardZonePlay:
					return ZoneChanger<Entity::kCardZonePlay, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardZonePutASide:
					return ZoneChanger<Entity::kCardZonePutASide, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardZoneRemoved:
					return ZoneChanger<Entity::kCardZoneRemoved, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardZoneSecret:
					return ZoneChanger<Entity::kCardZoneSecret, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				default:
					throw std::exception("Unknown card zone");
				}
			}

			template <Entity::CardZone ChangeToZone>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier, int pos)
			{
				switch (card_.GetZone())
				{
				case Entity::kCardZoneDeck:
					return ZoneChanger<Entity::kCardZoneDeck, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardZoneGraveyard:
					return ZoneChanger<Entity::kCardZoneGraveyard, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardZoneHand:
					return ZoneChanger<Entity::kCardZoneHand, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardZonePlay:
					return ZoneChanger<Entity::kCardZonePlay, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardZonePutASide:
					return ZoneChanger<Entity::kCardZonePutASide, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardZoneRemoved:
					return ZoneChanger<Entity::kCardZoneRemoved, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardZoneSecret:
					return ZoneChanger<Entity::kCardZoneSecret, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				default:
					throw std::exception("Unknown card zone");
				}
			}

			void Add(State::State & state)
			{
				switch (card_.GetZone())
				{
				case Entity::kCardZoneDeck:
					return ZoneChanger<Entity::kCardZoneDeck, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardZoneGraveyard:
					return ZoneChanger<Entity::kCardZoneGraveyard, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardZoneHand:
					return ZoneChanger<Entity::kCardZoneHand, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardZonePlay:
					return ZoneChanger<Entity::kCardZonePlay, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardZonePutASide:
					return ZoneChanger<Entity::kCardZonePutASide, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardZoneRemoved:
					return ZoneChanger<Entity::kCardZoneRemoved, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardZoneSecret:
					return ZoneChanger<Entity::kCardZoneSecret, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				default:
					throw std::exception("Unknown card zone");
				}
			}

		private:
			EntitiesManager & mgr_;
			CardRef card_ref_;
			Entity::Card & card_;
		};

		class ZoneChangerWithUnknownZoneUnknownType
		{
		public:
			ZoneChangerWithUnknownZoneUnknownType(EntitiesManager& mgr, CardRef card_ref, Entity::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

			template <Entity::CardZone ChangeToZone>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier)
			{
				switch (card_.GetCardType())
				{
				case Entity::kCardTypeMinion:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeMinion>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardTypeHeroPower:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeHeroPower>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardTypeSecret:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeSecret>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardTypeSpell:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeSpell>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case Entity::kCardTypeWeapon:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeWeapon>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				default:
					throw std::exception("unknown card type");
				}
			}

			template <Entity::CardZone ChangeToZone>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier, int pos)
			{
				switch (card_.GetCardType())
				{
				case Entity::kCardTypeHero:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeHero>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardTypeMinion:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeMinion>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardTypeHeroPower:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeHeroPower>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardTypeSecret:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeSecret>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardTypeSpell:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeSpell>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case Entity::kCardTypeWeapon:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeWeapon>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				default:
					throw std::exception("unknown card type");
				}
			}

			void Add(State::State & state)
			{
				switch (card_.GetCardType())
				{
				case Entity::kCardTypeHero:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeHero>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardTypeMinion:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeMinion>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardTypeHeroPower:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeHeroPower>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardTypeSecret:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeSecret>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardTypeSpell:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeSpell>(mgr_, card_ref_, card_).Add(state);
				case Entity::kCardTypeWeapon:
					return ZoneChangerWithUnknownZone<Entity::kCardTypeWeapon>(mgr_, card_ref_, card_).Add(state);
				default:
					throw std::exception("unknown card type");
				}
			}

		private:
			EntitiesManager & mgr_;
			CardRef card_ref_;
			Entity::Card & card_;
		};
	}
}