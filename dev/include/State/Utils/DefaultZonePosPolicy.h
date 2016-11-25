#pragma once

#include "State/Player.h"
#include "State/Types.h"

namespace state
{
	namespace Utils
	{
		template <CardZone Zone, CardType Type>
		struct ForcelyUseDefaultZonePos;

		template <CardZone Zone, CardType Type>
		class DefaultZonePosGetter;

		// Deck
		template <CardType Type>
		struct ForcelyUseDefaultZonePos<kCardZoneDeck, Type>
		{
			static constexpr bool value = true;
		};
		template <CardType Type>
		class DefaultZonePosGetter<kCardZoneDeck, Type>
		{
		public:
			int operator()(Player & player) const
			{
				return player.deck_.Size();
			}
		};

		// Hand
		template <CardType Type>
		struct ForcelyUseDefaultZonePos<kCardZoneHand, Type>
		{
			static constexpr bool value = true;
		};
		template <CardType Type>
		class DefaultZonePosGetter<kCardZoneHand, Type>
		{
		public:
			int operator()(Player & player) const
			{
				return player.hand_.Size();
			}
		};

		// Play - Weapon
		template <>
		struct ForcelyUseDefaultZonePos<kCardZonePlay, kCardTypeWeapon>
		{
			static constexpr bool value = true;
		};
		template <>
		class DefaultZonePosGetter<kCardZonePlay, kCardTypeWeapon>
		{
		public:
			constexpr int operator()(Player & player) const
			{
				return 0;
			}
		};

		// Play - Secrets
		template <>
		struct ForcelyUseDefaultZonePos<kCardZonePlay, kCardTypeSecret>
		{
			static constexpr bool value = true;
		};
		template <>
		class DefaultZonePosGetter<kCardZonePlay, kCardTypeSecret>
		{
		public:
			constexpr int operator()(Player & player) const
			{
				return -1; // do not specify position
			}
		};

		// Graveyard
		template <CardType Type>
		struct ForcelyUseDefaultZonePos<kCardZoneGraveyard, Type>
		{
			static constexpr bool value = true;
		};
		template <>
		class DefaultZonePosGetter<kCardZoneGraveyard, kCardTypeMinion>
		{
		public:
			int operator()(Player & player) const
			{
				return (int)player.graveyard_.GetTotalMinions();
			}
		};
		template <>
		class DefaultZonePosGetter<kCardZoneGraveyard, kCardTypeSpell>
		{
		public:
			int operator()(Player & player) const
			{
				return (int)player.graveyard_.GetTotalSpells();
			}
		};
		template <CardType Type>
		class DefaultZonePosGetter<kCardZoneGraveyard, Type>
		{
		public:
			int operator()(Player & player) const
			{
				return (int)player.graveyard_.GetTotalOthers();
			}
		};

		// default
		template <CardZone Zone, CardType Type>
		struct ForcelyUseDefaultZonePos
		{
			static constexpr bool value = false;
		};
	}
}