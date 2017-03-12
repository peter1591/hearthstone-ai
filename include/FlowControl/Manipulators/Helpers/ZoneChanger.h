#pragma once

#include <assert.h>
#include "state/Types.h"
#include "state/Cards/Card.h"
#include "state/board/Player.h"
#include "state/board/DefaultZonePosPolicy.h"
#include "FlowControl/Manipulators/Helpers/OrderedCardsManager.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			template <state::CardZone ChangingCardZone, state::CardType ChangingCardType>
			class ZoneChanger
			{
			public:
				ZoneChanger(state::State & state, state::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card)
					: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
				{
				}

				template <state::CardZone ChangeToZone>
				void ChangeTo(state::PlayerIdentifier player_identifier)
				{
					constexpr bool valid = state::board::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value;
					assert(valid);

					state::board::Player & player = state_.board.Get(card_.GetPlayerIdentifier());
					int new_pos = state::board::DefaultZonePosGetter<ChangeToZone, ChangingCardType>()(player);
					return ChangeToInternal<ChangeToZone>(player_identifier, new_pos);
				}

				template <state::CardZone ChangeToZone>
				void ChangeTo(state::PlayerIdentifier player_identifier, int pos)
				{
					constexpr bool valid = !state::board::ForcelyUseDefaultZonePos<ChangeToZone, ChangingCardType>::value;
					assert(valid);

					return ChangeToInternal<ChangeToZone>(player_identifier, pos);
				}

				void Add()
				{
					PlayerDataStructureMaintainer<ChangingCardType, ChangingCardZone>::Add(state_, flow_context_, card_ref_, card_);
				}

			private:
				template <state::CardZone ChangeToZone>
				void ChangeToInternal(state::PlayerIdentifier player_identifier, int pos)
				{
					PlayerDataStructureMaintainer<ChangingCardType, ChangingCardZone>::Remove(state_, flow_context_, card_ref_, card_);

					card_.SetLocation()
						.Player(player_identifier)
						.Zone(ChangeToZone)
						.Position(pos);

					PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::Add(state_, flow_context_, card_ref_, card_);
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

				template <state::CardZone ChangeToZone>
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
					default:
						throw std::exception("Unknown card zone");
					}
				}

				template <state::CardZone ChangeToZone>
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
					default:
						throw std::exception("Unknown card zone");
					}
				}

				void Add()
				{
					switch (card_.GetZone())
					{
					case state::kCardZoneDeck:
						return ZoneChanger<state::kCardZoneDeck, ChangingCardType>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardZoneGraveyard:
						return ZoneChanger<state::kCardZoneGraveyard, ChangingCardType>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardZoneHand:
						return ZoneChanger<state::kCardZoneHand, ChangingCardType>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardZonePlay:
						return ZoneChanger<state::kCardZonePlay, ChangingCardType>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardZoneSetASide:
						return ZoneChanger<state::kCardZoneSetASide, ChangingCardType>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardZoneRemoved:
						return ZoneChanger<state::kCardZoneRemoved, ChangingCardType>(state_, flow_context_, card_ref_, card_).Add();
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

				void Add()
				{
					switch (card_.GetCardType())
					{
					case state::kCardTypeHero:
						return ZoneChangerWithUnknownZone<state::kCardTypeHero>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardTypeMinion:
						return ZoneChangerWithUnknownZone<state::kCardTypeMinion>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardTypeHeroPower:
						return ZoneChangerWithUnknownZone<state::kCardTypeHeroPower>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardTypeSecret:
						return ZoneChangerWithUnknownZone<state::kCardTypeSecret>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardTypeSpell:
						return ZoneChangerWithUnknownZone<state::kCardTypeSpell>(state_, flow_context_, card_ref_, card_).Add();
					case state::kCardTypeWeapon:
						return ZoneChangerWithUnknownZone<state::kCardTypeWeapon>(state_, flow_context_, card_ref_, card_).Add();
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