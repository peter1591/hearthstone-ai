#pragma once

#include "state/board/Player.h"
#include "state/Types.h"

namespace state
{
	namespace board
	{
		template <CardZone Zone, CardType Type>
		struct ForcelyUseDefaultZonePos
		{
			static constexpr bool value = false;
		};

		template <CardZone Zone, CardType Type>
		class DefaultZonePosGetter
		{
		public:
			int operator()(Player & player) const
			{
				assert(false);
				return 0;
			}
		};

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
				return (int)player.hand_.Size();
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
			constexpr int operator()(Player &) const
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
			constexpr int operator()(Player &) const
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
	}
}