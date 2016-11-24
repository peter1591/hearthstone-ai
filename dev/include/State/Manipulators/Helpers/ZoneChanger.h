#pragma once

#include "State/Types.h"
#include "State/Cards/Card.h"
#include "State/State.h"
#include "State/Board/Player.h"
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
				ZoneChanger(Cards::Manager& mgr, CardRef card_ref, Cards::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

				template <CardZone ChangeToZone,
					typename std::enable_if_t<Utils::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value, nullptr_t> = nullptr>
					void ChangeTo(State & state, PlayerIdentifier player_identifier)
				{
					Player & player = state.board.players.Get(card_.GetPlayerIdentifier());
					int new_pos = Utils::DefaultZonePosGetter<ChangeToZone, ChangingCardType>()(player);
					return ChangeToInternal<ChangeToZone>(state, player_identifier, new_pos);
				}

				template <CardZone ChangeToZone,
					typename std::enable_if_t<!Utils::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value, nullptr_t> = nullptr>
					void ChangeTo(State & state, PlayerIdentifier player_identifier, int pos)
				{
					return ChangeToInternal<ChangeToZone>(state, player_identifier, pos);
				}

				void Add(State & state)
				{
					Events::AddToZoneEvent<ChangingCardType, ChangingCardZone>::Trigger(state, card_ref_, card_);
				}

			private:
				template <CardZone ChangeToZone>
				void ChangeToInternal(State & state, PlayerIdentifier player_identifier, int pos)
				{
					Events::RemovedFromZoneEvent<ChangingCardType, ChangingCardZone>::Trigger(state, card_ref_, card_);

					card_.SetLocation()
						.Player(player_identifier)
						.Zone(ChangeToZone)
						.Position(pos);

					Events::AddToZoneEvent<ChangingCardType, ChangeToZone>::Trigger(state, card_ref_, card_);
				}

			private:
				Cards::Manager & mgr_;
				CardRef card_ref_;
				Cards::Card & card_;
			};

			template <CardType ChangingCardType>
			class ZoneChangerWithUnknownZone
			{
			public:
				ZoneChangerWithUnknownZone(Cards::Manager& mgr, CardRef card_ref, Cards::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

				template <CardZone ChangeToZone>
				void ChangeTo(State & state, PlayerIdentifier player_identifier)
				{
					switch (card_.GetZone())
					{
					case kCardZoneDeck:
						return ZoneChanger<kCardZoneDeck, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardZoneGraveyard:
						return ZoneChanger<kCardZoneGraveyard, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardZoneHand:
						return ZoneChanger<kCardZoneHand, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardZonePlay:
						return ZoneChanger<kCardZonePlay, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardZonePutASide:
						return ZoneChanger<kCardZonePutASide, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardZoneRemoved:
						return ZoneChanger<kCardZoneRemoved, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardZoneSecret:
						return ZoneChanger<kCardZoneSecret, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					default:
						throw std::exception("Unknown card zone");
					}
				}

				template <CardZone ChangeToZone>
				void ChangeTo(State & state, PlayerIdentifier player_identifier, int pos)
				{
					switch (card_.GetZone())
					{
					case kCardZoneDeck:
						return ZoneChanger<kCardZoneDeck, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardZoneGraveyard:
						return ZoneChanger<kCardZoneGraveyard, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardZoneHand:
						return ZoneChanger<kCardZoneHand, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardZonePlay:
						return ZoneChanger<kCardZonePlay, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardZonePutASide:
						return ZoneChanger<kCardZonePutASide, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardZoneRemoved:
						return ZoneChanger<kCardZoneRemoved, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardZoneSecret:
						return ZoneChanger<kCardZoneSecret, ChangingCardType>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					default:
						throw std::exception("Unknown card zone");
					}
				}

				void Add(State & state)
				{
					switch (card_.GetZone())
					{
					case kCardZoneDeck:
						return ZoneChanger<kCardZoneDeck, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
					case kCardZoneGraveyard:
						return ZoneChanger<kCardZoneGraveyard, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
					case kCardZoneHand:
						return ZoneChanger<kCardZoneHand, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
					case kCardZonePlay:
						return ZoneChanger<kCardZonePlay, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
					case kCardZonePutASide:
						return ZoneChanger<kCardZonePutASide, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
					case kCardZoneRemoved:
						return ZoneChanger<kCardZoneRemoved, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
					case kCardZoneSecret:
						return ZoneChanger<kCardZoneSecret, ChangingCardType>(mgr_, card_ref_, card_).Add(state);
					default:
						throw std::exception("Unknown card zone");
					}
				}

			private:
				Cards::Manager & mgr_;
				CardRef card_ref_;
				Cards::Card & card_;
			};

			class ZoneChangerWithUnknownZoneUnknownType
			{
			public:
				ZoneChangerWithUnknownZoneUnknownType(Cards::Manager& mgr, CardRef card_ref, Cards::Card &card) : mgr_(mgr), card_ref_(card_ref), card_(card) {}

				template <CardZone ChangeToZone>
				void ChangeTo(State & state, PlayerIdentifier player_identifier)
				{
					switch (card_.GetCardType())
					{
					case kCardTypeMinion:
						return ZoneChangerWithUnknownZone<kCardTypeMinion>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardTypeHeroPower:
						return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardTypeSecret:
						return ZoneChangerWithUnknownZone<kCardTypeSecret>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardTypeSpell:
						return ZoneChangerWithUnknownZone<kCardTypeSpell>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					case kCardTypeWeapon:
						return ZoneChangerWithUnknownZone<kCardTypeWeapon>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier);
					default:
						throw std::exception("unknown card type");
					}
				}

				template <CardZone ChangeToZone>
				void ChangeTo(State & state, PlayerIdentifier player_identifier, int pos)
				{
					switch (card_.GetCardType())
					{
					case kCardTypeHero:
						return ZoneChangerWithUnknownZone<kCardTypeHero>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardTypeMinion:
						return ZoneChangerWithUnknownZone<kCardTypeMinion>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardTypeHeroPower:
						return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardTypeSecret:
						return ZoneChangerWithUnknownZone<kCardTypeSecret>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardTypeSpell:
						return ZoneChangerWithUnknownZone<kCardTypeSpell>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					case kCardTypeWeapon:
						return ZoneChangerWithUnknownZone<kCardTypeWeapon>(mgr_, card_ref_, card_).ChangeTo<ChangeToZone>(state, player_identifier, pos);
					default:
						throw std::exception("unknown card type");
					}
				}

				void Add(State & state)
				{
					switch (card_.GetCardType())
					{
					case kCardTypeHero:
						return ZoneChangerWithUnknownZone<kCardTypeHero>(mgr_, card_ref_, card_).Add(state);
					case kCardTypeMinion:
						return ZoneChangerWithUnknownZone<kCardTypeMinion>(mgr_, card_ref_, card_).Add(state);
					case kCardTypeHeroPower:
						return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(mgr_, card_ref_, card_).Add(state);
					case kCardTypeSecret:
						return ZoneChangerWithUnknownZone<kCardTypeSecret>(mgr_, card_ref_, card_).Add(state);
					case kCardTypeSpell:
						return ZoneChangerWithUnknownZone<kCardTypeSpell>(mgr_, card_ref_, card_).Add(state);
					case kCardTypeWeapon:
						return ZoneChangerWithUnknownZone<kCardTypeWeapon>(mgr_, card_ref_, card_).Add(state);
					default:
						throw std::exception("unknown card type");
					}
				}

			private:
				Cards::Manager & mgr_;
				CardRef card_ref_;
				Cards::Card & card_;
			};
		}
	}
}