#pragma once

#include <assert.h>
#include "state/Types.h"
#include "state/Cards/Card.h"
#include "state/board/Player.h"

namespace state {
	template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer;

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneInvalid> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneRemoved> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneSetASide> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneDeck>
	{
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneGraveyard>
	{
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardType TargetCardType>
	struct PlayerDataStructureMaintainer<TargetCardType, kCardZoneHand>
	{
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <>
	struct PlayerDataStructureMaintainer<kCardTypeHero, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeMinion, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = true;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card, int pos);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeSecret, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeWeapon, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeSpell, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeHeroPower, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};
	template <>
	struct PlayerDataStructureMaintainer<kCardTypeEnchantment, kCardZonePlay> {
		static constexpr bool SpecifyAddPosition = false;
		static void Add(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
		static void Remove(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card & card);
	};

	template <CardZone ChangingCardZone, CardType ChangingCardType>
	class ZoneChanger
	{
	public:
		ZoneChanger(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card &card)
			: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
		{
			assert(card.GetZone() == ChangingCardZone);
			assert(card.GetCardType() == ChangingCardType);
		}

		template <CardZone ChangeToZone,
			typename = std::enable_if_t<PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == false>>
			void ChangeTo(PlayerIdentifier player_identifier)
		{
			PlayerDataStructureMaintainer<ChangingCardType, ChangingCardZone>::Remove(state_, flow_context_, card_ref_, card_);

			card_.SetLocation()
				.Player(player_identifier)
				.Zone(ChangeToZone);

			PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::Add(state_, flow_context_, card_ref_, card_);
		}

		template <CardZone ChangeToZone,
			typename = std::enable_if_t<PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == true>>
			void ChangeTo(PlayerIdentifier player_identifier, int pos)
		{
			PlayerDataStructureMaintainer<ChangingCardType, ChangingCardZone>::Remove(state_, flow_context_, card_ref_, card_);

			card_.SetLocation()
				.Player(player_identifier)
				.Zone(ChangeToZone);

			PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::Add(state_, flow_context_, card_ref_, card_, pos);
		}

	private:
		State & state_;
		FlowContext & flow_context_;
		CardRef card_ref_;
		Cards::Card & card_;
	};

	template <CardType ChangingCardType>
	class ZoneChangerWithUnknownZone
	{
	public:
		ZoneChangerWithUnknownZone(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card &card)
			: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
		{
			assert(card.GetCardType() == ChangingCardType);
		}

		template <typename CardZone KnownZone>
		ZoneChanger<KnownZone, ChangingCardType> WithZone() {
			return ZoneChanger<KnownZone, ChangingCardType>(state_, flow_context_, card_ref_, card_);
		}

		template <CardZone ChangeToZone,
			typename = std::enable_if_t<PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == false>>
			void ChangeTo(PlayerIdentifier player_identifier)
		{
			switch (card_.GetZone())
			{
			case kCardZoneDeck:
				return ZoneChanger<kCardZoneDeck, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardZoneGraveyard:
				return ZoneChanger<kCardZoneGraveyard, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardZoneHand:
				return ZoneChanger<kCardZoneHand, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardZonePlay:
				return ZoneChanger<kCardZonePlay, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardZoneSetASide:
				return ZoneChanger<kCardZoneSetASide, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardZoneRemoved:
				return ZoneChanger<kCardZoneRemoved, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardZoneInvalid:
				return ZoneChanger<kCardZoneInvalid, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			default:
				throw std::exception("Unknown card zone");
			}
		}

		template <CardZone ChangeToZone,
			typename = std::enable_if_t<PlayerDataStructureMaintainer<ChangingCardType, ChangeToZone>::SpecifyAddPosition == true>>
			void ChangeTo(PlayerIdentifier player_identifier, int pos)
		{
			switch (card_.GetZone())
			{
			case kCardZoneDeck:
				return ZoneChanger<kCardZoneDeck, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardZoneGraveyard:
				return ZoneChanger<kCardZoneGraveyard, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardZoneHand:
				return ZoneChanger<kCardZoneHand, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardZonePlay:
				return ZoneChanger<kCardZonePlay, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardZoneSetASide:
				return ZoneChanger<kCardZoneSetASide, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardZoneRemoved:
				return ZoneChanger<kCardZoneRemoved, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardZoneInvalid:
				return ZoneChanger<kCardZoneInvalid, ChangingCardType>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			default:
				throw std::exception("Unknown card zone");
			}
		}

	private:
		State & state_;
		FlowContext & flow_context_;
		CardRef card_ref_;
		Cards::Card & card_;
	};

	template <CardZone ChangingCardZone>
	class ZoneChangerWithUnknownType
	{
	public:
		ZoneChangerWithUnknownType(State & state, FlowContext & flow_context, CardRef card_ref, Cards::Card &card)
			: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
		{
			assert(card.GetZone() == ChangingCardZone);
		}

		template <typename CardType KnownType>
		ZoneChanger<ChangingCardZone, KnownType> WithType() {
			return ZoneChanger<ChangingCardZone, KnownType>(state_, flow_context_, card_ref_, card_);
		}

		template <CardZone ChangeToZone>
		void ChangeTo(PlayerIdentifier player_identifier)
		{
			switch (card_.GetCardType())
			{
			case kCardTypeHero:
				return ZoneChanger<ChangingCardZone, kCardTypeHero>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeHeroPower:
				return ZoneChanger<ChangingCardZone, kCardTypeHeroPower>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeMinion:
				return ZoneChanger<ChangingCardZone, kCardTypeMinion>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeSecret:
				return ZoneChanger<ChangingCardZone, kCardTypeSecret>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeSpell:
				return ZoneChanger<ChangingCardZone, kCardTypeSpell>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeWeapon:
				return ZoneChanger<ChangingCardZone, kCardTypeWeapon>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeEnchantment:
				return ZoneChanger<ChangingCardZone, kCardTypeEnchantment>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
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
				return ZoneChanger<ChangingCardZone, kCardTypeHero>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeHeroPower:
				return ZoneChanger<ChangingCardZone, kCardTypeHeroPower>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeMinion:
				return ZoneChanger<ChangingCardZone, kCardTypeMinion>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeSecret:
				return ZoneChanger<ChangingCardZone, kCardTypeSecret>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeSpell:
				return ZoneChanger<ChangingCardZone, kCardTypeSpell>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeWeapon:
				return ZoneChanger<ChangingCardZone, kCardTypeWeapon>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeEnchantment:
				return ZoneChanger<ChangingCardZone, kCardTypeEnchantment>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			default:
				throw std::exception("unknown card type");
			}
		}

		template <>
		void ChangeTo<kCardZonePlay>(PlayerIdentifier player_identifier)
		{
			static constexpr CardZone ChangeToZone = kCardZonePlay;
			switch (card_.GetCardType())
			{
			case kCardTypeHero:
				return ZoneChanger<ChangingCardZone, kCardTypeHero>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeHeroPower:
				return ZoneChanger<ChangingCardZone, kCardTypeHeroPower>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeMinion:
				assert(false); // should specify position
			case kCardTypeSecret:
				return ZoneChanger<ChangingCardZone, kCardTypeSecret>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeSpell:
				return ZoneChanger<ChangingCardZone, kCardTypeSpell>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeWeapon:
				return ZoneChanger<ChangingCardZone, kCardTypeWeapon>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeEnchantment:
				return ZoneChanger<ChangingCardZone, kCardTypeEnchantment>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			default:
				throw std::exception("unknown card type");
			}
		}
		template <>
		void ChangeTo<kCardZonePlay>(PlayerIdentifier player_identifier, int pos)
		{
			static constexpr CardZone ChangeToZone = kCardZonePlay;
			switch (card_.GetCardType())
			{
			case kCardTypeHero:
				assert(false); // should not specify position
			case kCardTypeHeroPower:
				assert(false); // should not specify position
			case kCardTypeMinion:
				return ZoneChanger<ChangingCardZone, kCardTypeMinion>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeSecret:
				assert(false); // should not specify position
			case kCardTypeSpell:
				assert(false); // should not specify position
			case kCardTypeWeapon:
				assert(false); // should not specify position
			case kCardTypeEnchantment:
				assert(false); // should not specify position
			default:
				throw std::exception("unknown card type");
			}
		}

	private:
		State & state_;
		FlowContext & flow_context_;
		CardRef card_ref_;
		Cards::Card & card_;
	};

	class ZoneChangerWithUnknownZoneUnknownType
	{
	public:
		ZoneChangerWithUnknownZoneUnknownType(State& state, FlowContext & flow_context, CardRef card_ref, Cards::Card &card)
			: state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
		{}

		template <typename CardZone KnownZone>
		ZoneChangerWithUnknownType<KnownZone> WithZone() {
			return ZoneChangerWithUnknownType<KnownZone>(state_, flow_context_, card_ref_, card_);
		}

		template <typename CardType KnownType>
		ZoneChangerWithUnknownZone<KnownType> WithType() {
			return ZoneChangerWithUnknownZone<KnownType>(state_, flow_context_, card_ref_, card_);
		}

		template <CardZone ChangeToZone>
		void ChangeTo(PlayerIdentifier player_identifier)
		{
			switch (card_.GetCardType())
			{
			case kCardTypeMinion:
				return ZoneChangerWithUnknownZone<kCardTypeMinion>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeHeroPower:
				return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeSecret:
				return ZoneChangerWithUnknownZone<kCardTypeSecret>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeSpell:
				return ZoneChangerWithUnknownZone<kCardTypeSpell>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
			case kCardTypeWeapon:
				return ZoneChangerWithUnknownZone<kCardTypeWeapon>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier);
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
				return ZoneChangerWithUnknownZone<kCardTypeHero>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeMinion:
				return ZoneChangerWithUnknownZone<kCardTypeMinion>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeHeroPower:
				return ZoneChangerWithUnknownZone<kCardTypeHeroPower>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeSecret:
				return ZoneChangerWithUnknownZone<kCardTypeSecret>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeSpell:
				return ZoneChangerWithUnknownZone<kCardTypeSpell>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			case kCardTypeWeapon:
				return ZoneChangerWithUnknownZone<kCardTypeWeapon>(state_, flow_context_, card_ref_, card_).ChangeTo<ChangeToZone>(player_identifier, pos);
			default:
				throw std::exception("unknown card type");
			}
		}

	private:
		State & state_;
		FlowContext & flow_context_;
		CardRef card_ref_;
		Cards::Card & card_;
	};
}