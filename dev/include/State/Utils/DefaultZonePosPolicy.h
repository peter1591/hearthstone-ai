#pragma once

#include "State/Board/Player.h"
#include "Entity/CardZone.h"
#include "Entity/CardType.h"

namespace State
{
	namespace Utils
	{
		template <Entity::CardZone Zone, Entity::CardType Type>
		struct ForcelyUseDefaultZonePos;

		template <Entity::CardZone Zone, Entity::CardType Type>
		class DefaultZonePosGetter;

		// Deck
		template <Entity::CardType Type>
		struct ForcelyUseDefaultZonePos<Entity::kCardZoneDeck, Type>
		{
			static constexpr bool value = true;
		};
		template <Entity::CardType Type>
		class DefaultZonePosGetter<Entity::kCardZoneDeck, Type>
		{
		public:
			int operator()(Player & player) const
			{
				return player.deck_.Size();
			}
		};

		// Hand
		template <Entity::CardType Type>
		struct ForcelyUseDefaultZonePos<Entity::kCardZoneHand, Type>
		{
			static constexpr bool value = true;
		};
		template <Entity::CardType Type>
		class DefaultZonePosGetter<Entity::kCardZoneHand, Type>
		{
		public:
			int operator()(Player & player) const
			{
				return player.hand_.Size();
			}
		};

		// Play - Weapon
		template <>
		struct ForcelyUseDefaultZonePos<Entity::kCardZonePlay, Entity::kCardTypeWeapon>
		{
			static constexpr bool value = true;
		};
		template <>
		class DefaultZonePosGetter<Entity::kCardZonePlay, Entity::kCardTypeWeapon>
		{
		public:
			constexpr int operator()(Player & player) const
			{
				return 0;
			}
		};

		// Play - Secrets
		template <>
		struct ForcelyUseDefaultZonePos<Entity::kCardZonePlay, Entity::kCardTypeSecret>
		{
			static constexpr bool value = true;
		};
		template <>
		class DefaultZonePosGetter<Entity::kCardZonePlay, Entity::kCardTypeSecret>
		{
		public:
			constexpr int operator()(Player & player) const
			{
				return -1; // do not specify position
			}
		};

		// Graveyard
		template <Entity::CardType Type>
		struct ForcelyUseDefaultZonePos<Entity::kCardZoneGraveyard, Type>
		{
			static constexpr bool value = true;
		};
		template <>
		class DefaultZonePosGetter<Entity::kCardZoneGraveyard, Entity::kCardTypeMinion>
		{
		public:
			int operator()(Player & player) const
			{
				return (int)player.graveyard_.GetTotalMinions();
			}
		};
		template <>
		class DefaultZonePosGetter<Entity::kCardZoneGraveyard, Entity::kCardTypeSpell>
		{
		public:
			int operator()(Player & player) const
			{
				return (int)player.graveyard_.GetTotalSpells();
			}
		};
		template <Entity::CardType Type>
		class DefaultZonePosGetter<Entity::kCardZoneGraveyard, Type>
		{
		public:
			int operator()(Player & player) const
			{
				return (int)player.graveyard_.GetTotalOthers();
			}
		};

		// default
		template <Entity::CardZone Zone, Entity::CardType Type>
		struct ForcelyUseDefaultZonePos
		{
			static constexpr bool value = false;
		};
	}
}