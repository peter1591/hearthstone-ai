#pragma once

#include "State/Types.h"
#include "State/Cards/Card.h"
#include "State/CardRef.h"
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
		template <State::CardZone ChangingCardZone, State::CardType ChangingCardType>
		class ZoneChanger
		{
		public:
			ZoneChanger(State::Cards::Manager& mgr, CardRef card_ref, State::Cards::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

			template <State::CardZone ChangeToZone,
				typename std::enable_if_t<State::Utils::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value, nullptr_t> = nullptr>
				void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier)
			{
				State::Player & player = state.board.players.Get(card_.GetPlayerIdentifier());
				int new_pos = State::Utils::DefaultZonePosGetter<ChangeToZone, ChangingCardType>()(player);
				return ChangeToInternal<ChangeToZone>(state, player_identifier, new_pos);
			}

			template <State::CardZone ChangeToZone,
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
			template <State::CardZone ChangeToZone>
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
			State::Cards::Manager & mgr_;
			CardRef card_ref_;
			State::Cards::Card & card_;
		};

		template <State::CardType ChangingCardType>
		class ZoneChangerWithUnknownZone
		{
		public:
			ZoneChangerWithUnknownZone(State::Cards::Manager& mgr, CardRef card_ref, State::Cards::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

			template <State::CardZone ChangeToZone>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier)
			{
				switch (card_.GetZone())
				{
				case State::kCardZoneDeck:
					return ZoneChanger<State::kCardZoneDeck, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardZoneGraveyard:
					return ZoneChanger<State::kCardZoneGraveyard, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardZoneHand:
					return ZoneChanger<State::kCardZoneHand, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardZonePlay:
					return ZoneChanger<State::kCardZonePlay, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardZonePutASide:
					return ZoneChanger<State::kCardZonePutASide, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardZoneRemoved:
					return ZoneChanger<State::kCardZoneRemoved, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardZoneSecret:
					return ZoneChanger<State::kCardZoneSecret, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				default:
					throw std::exception("Unknown card zone");
				}
			}

			template <State::CardZone ChangeToZone>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier, int pos)
			{
				switch (card_.GetZone())
				{
				case State::kCardZoneDeck:
					return ZoneChanger<State::kCardZoneDeck, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardZoneGraveyard:
					return ZoneChanger<State::kCardZoneGraveyard, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardZoneHand:
					return ZoneChanger<State::kCardZoneHand, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardZonePlay:
					return ZoneChanger<State::kCardZonePlay, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardZonePutASide:
					return ZoneChanger<State::kCardZonePutASide, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardZoneRemoved:
					return ZoneChanger<State::kCardZoneRemoved, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardZoneSecret:
					return ZoneChanger<State::kCardZoneSecret, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				default:
					throw std::exception("Unknown card zone");
				}
			}

			void Add(State::State & state)
			{
				switch (card_.GetZone())
				{
				case State::kCardZoneDeck:
					return ZoneChanger<State::kCardZoneDeck, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case State::kCardZoneGraveyard:
					return ZoneChanger<State::kCardZoneGraveyard, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case State::kCardZoneHand:
					return ZoneChanger<State::kCardZoneHand, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case State::kCardZonePlay:
					return ZoneChanger<State::kCardZonePlay, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case State::kCardZonePutASide:
					return ZoneChanger<State::kCardZonePutASide, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case State::kCardZoneRemoved:
					return ZoneChanger<State::kCardZoneRemoved, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				case State::kCardZoneSecret:
					return ZoneChanger<State::kCardZoneSecret, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
				default:
					throw std::exception("Unknown card zone");
				}
			}

		private:
			State::Cards::Manager & mgr_;
			CardRef card_ref_;
			State::Cards::Card & card_;
		};

		class ZoneChangerWithUnknownZoneUnknownType
		{
		public:
			ZoneChangerWithUnknownZoneUnknownType(State::Cards::Manager& mgr, CardRef card_ref, State::Cards::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

			template <State::CardZone ChangeToZone>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier)
			{
				switch (card_.GetCardType())
				{
				case State::kCardTypeMinion:
					return ZoneChangerWithUnknownZone<State::kCardTypeMinion>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardTypeHeroPower:
					return ZoneChangerWithUnknownZone<State::kCardTypeHeroPower>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardTypeSecret:
					return ZoneChangerWithUnknownZone<State::kCardTypeSecret>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardTypeSpell:
					return ZoneChangerWithUnknownZone<State::kCardTypeSpell>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				case State::kCardTypeWeapon:
					return ZoneChangerWithUnknownZone<State::kCardTypeWeapon>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
				default:
					throw std::exception("unknown card type");
				}
			}

			template <State::CardZone ChangeToZone>
			void ChangeTo(State::State & state, State::PlayerIdentifier player_identifier, int pos)
			{
				switch (card_.GetCardType())
				{
				case State::kCardTypeHero:
					return ZoneChangerWithUnknownZone<State::kCardTypeHero>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardTypeMinion:
					return ZoneChangerWithUnknownZone<State::kCardTypeMinion>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardTypeHeroPower:
					return ZoneChangerWithUnknownZone<State::kCardTypeHeroPower>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardTypeSecret:
					return ZoneChangerWithUnknownZone<State::kCardTypeSecret>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardTypeSpell:
					return ZoneChangerWithUnknownZone<State::kCardTypeSpell>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				case State::kCardTypeWeapon:
					return ZoneChangerWithUnknownZone<State::kCardTypeWeapon>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
				default:
					throw std::exception("unknown card type");
				}
			}

			void Add(State::State & state)
			{
				switch (card_.GetCardType())
				{
				case State::kCardTypeHero:
					return ZoneChangerWithUnknownZone<State::kCardTypeHero>(mgr_, card_ref_, card_).Add(state);
				case State::kCardTypeMinion:
					return ZoneChangerWithUnknownZone<State::kCardTypeMinion>(mgr_, card_ref_, card_).Add(state);
				case State::kCardTypeHeroPower:
					return ZoneChangerWithUnknownZone<State::kCardTypeHeroPower>(mgr_, card_ref_, card_).Add(state);
				case State::kCardTypeSecret:
					return ZoneChangerWithUnknownZone<State::kCardTypeSecret>(mgr_, card_ref_, card_).Add(state);
				case State::kCardTypeSpell:
					return ZoneChangerWithUnknownZone<State::kCardTypeSpell>(mgr_, card_ref_, card_).Add(state);
				case State::kCardTypeWeapon:
					return ZoneChangerWithUnknownZone<State::kCardTypeWeapon>(mgr_, card_ref_, card_).Add(state);
				default:
					throw std::exception("unknown card type");
				}
			}

		private:
			State::Cards::Manager & mgr_;
			CardRef card_ref_;
			State::Cards::Card & card_;
		};
	}
}