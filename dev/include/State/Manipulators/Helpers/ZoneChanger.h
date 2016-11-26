#pragma once

#include "State/Types.h"
#include "State/Cards/Card.h"
#include "State/State.h"
#include "State/board/Player.h"
#include "State/board/DefaultZonePosPolicy.h"

namespace state
{
	namespace Manipulators
	{
		namespace Helpers
		{
			template <CardType TargetCardType, CardZone TargetCardZone>
			class AddToPlayerDatStructure
			{
			public:
				static void Add(State & state, CardRef card_ref, Cards::Card & card)
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
					state.board.Get(card.GetPlayerIdentifier())
						.deck_.GetLocationManipulator().Insert(state, card_ref);
				}

				static void AddToHandZone(State & state, CardRef card_ref, Cards::Card & card)
				{
					state.board.Get(card.GetPlayerIdentifier())
						.hand_.GetLocationManipulator().Insert(state, card_ref);
				}

				static void AddToPlayZone(State & state, CardRef card_ref, Cards::Card & card)
				{
					board::Player & player = state.board.Get(card.GetPlayerIdentifier());

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
					state.board.Get(card.GetPlayerIdentifier())
						.graveyard_.GetLocationManipulator<TargetCardType>().Insert(state, card_ref);
				}
			};

			template <CardType RemovingCardType, CardZone RemovingCardZone>
			class RemoveFromPlayerDatStructure
			{
			public:
				static void Remove(State & state, CardRef card_ref, Cards::Card & card)
				{
					switch (RemovingCardZone)
					{
					case kCardZoneDeck:
						return RemoveFromDeckZone(state, card_ref, card);
					case kCardZoneHand:
						return RemoveFromHandZone(state, card_ref, card);
					case kCardZonePlay:
						return RemoveFromPlayZone(state, card_ref, card);
					case kCardZoneGraveyard:
						return RemoveFromGraveyardZone(state, card_ref, card);
						break;
					}
				}

			private:
				static void RemoveFromDeckZone(State & state, CardRef card_ref, Cards::Card & card)
				{
					state.board.Get(card.GetPlayerIdentifier())
						.deck_.GetLocationManipulator().Remove(state, card.GetZonePosition());
				}

				static void RemoveFromHandZone(State & state, CardRef card_ref, Cards::Card & card)
				{
					state.board.Get(card.GetPlayerIdentifier())
						.hand_.GetLocationManipulator().Remove(state, card.GetZonePosition());
				}

				static void RemoveFromPlayZone(State & state, CardRef card_ref, Cards::Card & card)
				{
					board::Player & player = state.board.Get(card.GetPlayerIdentifier());

					switch (RemovingCardType)
					{
					case kCardTypeMinion:
						return player.minions_.GetLocationManipulator().Remove(state, card.GetZonePosition());
					case kCardTypeWeapon:
						return player.weapon_.Destroy();
					case kCardTypeSecret:
						return player.secrets_.Remove(card.GetCardId());
					}
				}

				static void RemoveFromGraveyardZone(State & state, CardRef card_ref, Cards::Card & card)
				{
					state.board.Get(card.GetPlayerIdentifier())
						.graveyard_.GetLocationManipulator<RemovingCardType>().Remove(state, card.GetZonePosition());
				}
			};

			template <CardZone ChangingCardZone, CardType ChangingCardType>
			class ZoneChanger
			{
			public:
				ZoneChanger(State & state, CardRef card_ref, Cards::Card &card) : state_(state), card_ref_(card_ref), card_(card) {}

				template <CardZone ChangeToZone,
					typename std::enable_if_t<board::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value, nullptr_t> = nullptr>
					void ChangeTo(PlayerIdentifier player_identifier)
				{
					board::Player & player = state_.board.Get(card_.GetPlayerIdentifier());
					int new_pos = board::DefaultZonePosGetter<ChangeToZone, ChangingCardType>()(player);
					return ChangeToInternal<ChangeToZone>(player_identifier, new_pos);
				}

				template <CardZone ChangeToZone,
					typename std::enable_if_t<!board::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value, nullptr_t> = nullptr>
					void ChangeTo(PlayerIdentifier player_identifier, int pos)
				{
					return ChangeToInternal<ChangeToZone>(player_identifier, pos);
				}

				void Add()
				{
					AddToPlayerDatStructure<ChangingCardType, ChangingCardZone>::Add(state_, card_ref_, card_);
				}

			private:
				template <CardZone ChangeToZone>
				void ChangeToInternal(PlayerIdentifier player_identifier, int pos)
				{
					RemoveFromPlayerDatStructure<ChangingCardType, ChangingCardZone>::Remove(state_, card_ref_, card_);

					card_.SetLocation()
						.Player(player_identifier)
						.Zone(ChangeToZone)
						.Position(pos);

					AddToPlayerDatStructure<ChangingCardType, ChangeToZone>::Add(state_, card_ref_, card_);
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