#pragma once

#include <assert.h>
#include "state/Types.h"
#include "state/Cards/Card.h"
#include "state/board/Player.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			template <state::CardType TargetCardType, state::CardZone TargetCardZone> struct PlayerDataStructureMaintainer;

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneInvalid> {
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneRemoved> {
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneSetASide> {
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneDeck>
			{
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneGraveyard>
			{
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};

			template <state::CardType TargetCardType>
			struct PlayerDataStructureMaintainer<TargetCardType, state::kCardZoneHand>
			{
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};

			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeHero, state::kCardZonePlay> {
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeMinion, state::kCardZonePlay> {
				static constexpr bool SpecifyAddPosition = true;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, int pos);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeSecret, state::kCardZonePlay> {
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeWeapon, state::kCardZonePlay> {
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeSpell, state::kCardZonePlay> {
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};
			template <>
			struct PlayerDataStructureMaintainer<state::kCardTypeHeroPower, state::kCardZonePlay> {
				static constexpr bool SpecifyAddPosition = false;
				static void Add(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
				static void Remove(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);
			};

			template <state::CardZone ChangingCardZone, state::CardType ChangingCardType>
			class ZoneChanger
			{
			public:
				ZoneChanger(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
					: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{
				}

				template <state::CardZone ChangeToZone,
					typename = std::enable_if_t<PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == false>>
				void ChangeTo(state::PlayerIdentifier player_identifier)
				{
					PlayerDataStructureMaintainer<ChangingCardType, ChangingCardZone>::Remove(state_, flow_context_, card_ref_, card_);

					card_.SetLocation()
						.Player(player_identifier)
						.Zone(ChangeToZone);

					PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::Add(state_, flow_context_, card_ref_, card_);
				}

				template <state::CardZone ChangeToZone,
					typename = std::enable_if_t<PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == true>>
				void ChangeTo(state::PlayerIdentifier player_identifier, int pos)
				{
					PlayerDataStructureMaintainer<ChangingCardType, ChangingCardZone>::Remove(state_, flow_context_, card_ref_, card_);

					card_.SetLocation()
						.Player(player_identifier)
						.Zone(ChangeToZone);

					PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::Add(state_, flow_context_, card_ref_, card_, pos);
				}

			private:
				state::State & state_;
				state::FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};

			template <state::CardType ChangingCardType>
			class ZoneChangerWithUnknownZone
			{
			public:
				ZoneChangerWithUnknownZone(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
					: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{}

				template <state::CardZone ChangeToZone,
					typename = std::enable_if_t<PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == false>>
				void ChangeTo(state::PlayerIdentifier player_identifier)
				{
					switch (card_.GetZone())
					{
					case state::kCardZoneDeck:
						return ZoneChanger<state::kCardZoneDeck, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardZoneGraveyard:
						return ZoneChanger<state::kCardZoneGraveyard, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardZoneHand:
						return ZoneChanger<state::kCardZoneHand, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardZonePlay:
						return ZoneChanger<state::kCardZonePlay, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardZoneSetASide:
						return ZoneChanger<state::kCardZoneSetASide, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardZoneRemoved:
						return ZoneChanger<state::kCardZoneRemoved, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardZoneInvalid:
						return ZoneChanger<state::kCardZoneInvalid, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					default:
						throw std::exception("Unknown card zone");
					}
				}

				template <state::CardZone ChangeToZone,
					typename = std::enable_if_t<PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == true>>
				void ChangeTo(state::PlayerIdentifier player_identifier, int pos)
				{
					switch (card_.GetZone())
					{
					case state::kCardZoneDeck:
						return ZoneChanger<state::kCardZoneDeck, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardZoneGraveyard:
						return ZoneChanger<state::kCardZoneGraveyard, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardZoneHand:
						return ZoneChanger<state::kCardZoneHand, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardZonePlay:
						return ZoneChanger<state::kCardZonePlay, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardZoneSetASide:
						return ZoneChanger<state::kCardZoneSetASide, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardZoneRemoved:
						return ZoneChanger<state::kCardZoneRemoved, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardZoneInvalid:
						return ZoneChanger<state::kCardZoneInvalid, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					default:
						throw std::exception("Unknown card zone");
					}
				}

			private:
				state::State & state_;
				state::FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};

			class ZoneChangerWithUnknownZoneUnknownType
			{
			public:
				ZoneChangerWithUnknownZoneUnknownType(state::State& state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
					: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{}

				template <state::CardZone ChangeToZone>
				void ChangeTo(state::PlayerIdentifier player_identifier)
				{
					switch (card_.GetCardType())
					{
					case state::kCardTypeMinion:
						return ZoneChangerWithUnknownZone<state::kCardTypeMinion>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardTypeHeroPower:
						return ZoneChangerWithUnknownZone<state::kCardTypeHeroPower>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardTypeSecret:
						return ZoneChangerWithUnknownZone<state::kCardTypeSecret>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardTypeSpell:
						return ZoneChangerWithUnknownZone<state::kCardTypeSpell>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					case state::kCardTypeWeapon:
						return ZoneChangerWithUnknownZone<state::kCardTypeWeapon>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
					default:
						throw std::exception("unknown card type");
					}
				}

				template <state::CardZone ChangeToZone>
				void ChangeTo(state::PlayerIdentifier player_identifier, int pos)
				{
					switch (card_.GetCardType())
					{
					case state::kCardTypeHero:
						return ZoneChangerWithUnknownZone<state::kCardTypeHero>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardTypeMinion:
						return ZoneChangerWithUnknownZone<state::kCardTypeMinion>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardTypeHeroPower:
						return ZoneChangerWithUnknownZone<state::kCardTypeHeroPower>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardTypeSecret:
						return ZoneChangerWithUnknownZone<state::kCardTypeSecret>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardTypeSpell:
						return ZoneChangerWithUnknownZone<state::kCardTypeSpell>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					case state::kCardTypeWeapon:
						return ZoneChangerWithUnknownZone<state::kCardTypeWeapon>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
					default:
						throw std::exception("unknown card type");
					}
				}

			private:
				state::State & state_;
				state::FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::Card & card_;
			};
		}
	}
}