#pragma once

#include "State/Types.h"
#include "State/Cards/Card.h"
#include "State/State.h"
#include "State/Player.h"
#include "State/Utils/DefaultZonePosPolicy.h"
#include "State/Manipulators/Events/AddToZone.h"
#include "State/Manipulators/Events/RemovedFromZone.h"

namespace State
{
	namespace Manipulators
	{
		namespace Helpers
		{
			template <CardZone ChangingCardZone, CardType ChangingCardType>
			class ZoneChanger
			{
			public:
				ZoneChanger(State & state, CardRef card_ref, Cards::Card &card) : state_(state), card_ref_(card_ref), card_(card) {}

				template <CardZone ChangeToZone,
					typename std::enable_if_t<Utils::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value, nullptr_t> = nullptr>
					void ChangeTo(PlayerIdentifier player_identifier)
				{
					Player & player = state_.board.players.Get(card_.GetPlayerIdentifier());
					int new_pos = Utils::DefaultZonePosGetter<ChangeToZone, ChangingCardType>()(player);
					return ChangeToInternal<ChangeToZone>(player_identifier, new_pos);
				}

				template <CardZone ChangeToZone,
					typename std::enable_if_t<!Utils::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value, nullptr_t> = nullptr>
					void ChangeTo(PlayerIdentifier player_identifier, int pos)
				{
					return ChangeToInternal<ChangeToZone>(player_identifier, pos);
				}

				void Add()
				{
					Events::AddToZoneEvent<ChangingCardType, ChangingCardZone>::Trigger(state_, card_ref_, card_);
				}

			private:
				template <CardZone ChangeToZone>
				void ChangeToInternal(PlayerIdentifier player_identifier, int pos)
				{
					Events::RemovedFromZoneEvent<ChangingCardType, ChangingCardZone>::Trigger(state_, card_ref_, card_);

					card_.SetLocation()
						.Player(player_identifier)
						.Zone(ChangeToZone)
						.Position(pos);

					Events::AddToZoneEvent<ChangingCardType, ChangeToZone>::Trigger(state_, card_ref_, card_);
				}

			private:
				State & state_;
				CardRef card_ref_;
				Cards::Card & card_;
			};

			template <CardType ChangingCardType>
			class ZoneChangerWithUnknownZone
			{
			public:
				ZoneChangerWithUnknownZone(State & state, CardRef card_ref, Cards::Card &card) : state_(state), card_ref_(card_ref), card_(card) {}

				template <CardZone ChangeToZone>
				void ChangeTo(PlayerIdentifier player_identifier)
				{
					switch (card_.GetZone())
					{
					case kCardZoneDeck:
						return ZoneChanger<kCardZoneDeck, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardZoneGraveyard:
						return ZoneChanger<kCardZoneGraveyard, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardZoneHand:
						return ZoneChanger<kCardZoneHand, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardZonePlay:
						return ZoneChanger<kCardZonePlay, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardZonePutASide:
						return ZoneChanger<kCardZonePutASide, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardZoneRemoved:
						return ZoneChanger<kCardZoneRemoved, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardZoneSecret:
						return ZoneChanger<kCardZoneSecret, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					default:
						throw std::exception("Unknown card zone");
					}
				}

				template <CardZone ChangeToZone>
				void ChangeTo(PlayerIdentifier player_identifier, int pos)
				{
					switch (card_.GetZone())
					{
					case kCardZoneDeck:
						return ZoneChanger<kCardZoneDeck, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardZoneGraveyard:
						return ZoneChanger<kCardZoneGraveyard, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardZoneHand:
						return ZoneChanger<kCardZoneHand, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardZonePlay:
						return ZoneChanger<kCardZonePlay, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardZonePutASide:
						return ZoneChanger<kCardZonePutASide, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardZoneRemoved:
						return ZoneChanger<kCardZoneRemoved, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardZoneSecret:
						return ZoneChanger<kCardZoneSecret, ChangingCardType>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					default:
						throw std::exception("Unknown card zone");
					}
				}

				void Add()
				{
					switch (card_.GetZone())
					{
					case kCardZoneDeck:
						return ZoneChanger<kCardZoneDeck, ChangingCardType>(state_, card_ref_, card_).Add();
					case kCardZoneGraveyard:
						return ZoneChanger<kCardZoneGraveyard, ChangingCardType>(state_, card_ref_, card_).Add();
					case kCardZoneHand:
						return ZoneChanger<kCardZoneHand, ChangingCardType>(state_, card_ref_, card_).Add();
					case kCardZonePlay:
						return ZoneChanger<kCardZonePlay, ChangingCardType>(state_, card_ref_, card_).Add();
					case kCardZonePutASide:
						return ZoneChanger<kCardZonePutASide, ChangingCardType>(state_, card_ref_, card_).Add();
					case kCardZoneRemoved:
						return ZoneChanger<kCardZoneRemoved, ChangingCardType>(state_, card_ref_, card_).Add();
					case kCardZoneSecret:
						return ZoneChanger<kCardZoneSecret, ChangingCardType>(state_, card_ref_, card_).Add();
					default:
						throw std::exception("Unknown card zone");
					}
				}

			private:
				State & state_;
				CardRef card_ref_;
				Cards::Card & card_;
			};

			class ZoneChangerWithUnknownZoneUnknownType
			{
			public:
				ZoneChangerWithUnknownZoneUnknownType(State& state, CardRef card_ref, Cards::Card &card) : state_(state), card_ref_(card_ref), card_(card) {}

				template <CardZone ChangeToZone>
				void ChangeTo(PlayerIdentifier player_identifier)
				{
					switch (card_.GetCardType())
					{
					case kCardTypeMinion:
						return ZoneChangerWithUnknownZone<kCardTypeMinion>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardTypeHeroPower:
						return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardTypeSecret:
						return ZoneChangerWithUnknownZone<kCardTypeSecret>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardTypeSpell:
						return ZoneChangerWithUnknownZone<kCardTypeSpell>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case kCardTypeWeapon:
						return ZoneChangerWithUnknownZone<kCardTypeWeapon>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					default:
						throw std::exception("unknown card type");
					}
				}

				template <CardZone ChangeToZone>
				void ChangeTo(PlayerIdentifier player_identifier, int pos)
				{
					switch (card_.GetCardType())
					{
					case kCardTypeHero:
						return ZoneChangerWithUnknownZone<kCardTypeHero>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardTypeMinion:
						return ZoneChangerWithUnknownZone<kCardTypeMinion>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardTypeHeroPower:
						return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardTypeSecret:
						return ZoneChangerWithUnknownZone<kCardTypeSecret>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardTypeSpell:
						return ZoneChangerWithUnknownZone<kCardTypeSpell>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case kCardTypeWeapon:
						return ZoneChangerWithUnknownZone<kCardTypeWeapon>(state_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					default:
						throw std::exception("unknown card type");
					}
				}

				void Add()
				{
					switch (card_.GetCardType())
					{
					case kCardTypeHero:
						return ZoneChangerWithUnknownZone<kCardTypeHero>(state_, card_ref_, card_).Add();
					case kCardTypeMinion:
						return ZoneChangerWithUnknownZone<kCardTypeMinion>(state_, card_ref_, card_).Add();
					case kCardTypeHeroPower:
						return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(state_, card_ref_, card_).Add();
					case kCardTypeSecret:
						return ZoneChangerWithUnknownZone<kCardTypeSecret>(state_, card_ref_, card_).Add();
					case kCardTypeSpell:
						return ZoneChangerWithUnknownZone<kCardTypeSpell>(state_, card_ref_, card_).Add();
					case kCardTypeWeapon:
						return ZoneChangerWithUnknownZone<kCardTypeWeapon>(state_, card_ref_, card_).Add();
					default:
						throw std::exception("unknown card type");
					}
				}

			private:
				State & state_;
				CardRef card_ref_;
				Cards::Card & card_;
			};
		}
	}
}