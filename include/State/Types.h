#pragma once

#include <type_traits>
#include "Utils/CloneableContainers/Vector.h"

namespace state
{
	namespace Cards
	{
		class Card;
	}

	enum CardType
	{
		kCardTypeHero,
		kCardTypeMinion,
		kCardTypeSpell,
		kCardTypeWeapon,
		kCardTypeSecret,
		kCardTypeHeroPower,
		kCardTypeEnchantment,
		kCardTypeInvalid
	};

	enum CardZone
	{
		kCardZonePlay,
		kCardZoneDeck,
		kCardZoneHand,
		kCardZoneSecret,
		kCardZoneGraveyard,
		kCardZoneRemoved,
		kCardZoneSetASide,
		kCardZoneInvalid
	};

	enum CardRace
	{
		kCardRaceDemon,
		kCardRaceMech,
		kCardRaceDragon,
		kCardRaceBeast,
		kCardRacePirate,
		kCardRaceMurloc,
		kCardRaceTotem,
		kCardRaceInvalid
	};

	enum CardSet
	{
		kCardSetCore,
		kCardSetExpert1,
		kCardSetGVG,
		kCardSetTGT,
		kCardSetNaxx,
		kCardSetBRM,
		kCardSetLOE,
		kCardSetOldGods,
		kCardSetKara,
		kCardSetGangs,
		kCardSetTB,
		kCardSetInvalid
	};

	enum CardRarity
	{
		kCardRarityCommon,
		kCardRarityRare,
		kCardRarityEpic,
		kCardRarityLegendary,
		kCardRarityInvalid
	};

	enum PlayerIdentifier
	{
		kPlayerFirst,
		kPlayerSecond,
		kPlayerInvalid
	};

	class CardRef
	{
	public:
		typedef typename Utils::CloneableContainers::Vector<Cards::Card>::Identifier IdentifierType;

		CardRef() {}
		explicit CardRef(IdentifierType id) : id(id) {}

		bool operator==(CardRef rhs) const
		{
			return this->id == rhs.id;
		}

		bool operator!=(CardRef rhs) const
		{
			return !(*this == rhs);
		}

		bool IsValid() const { return id.IsValid(); }
		void Invalidate() { id = IdentifierType::GetInvalidIdentifier(); }

		IdentifierType id;
	};
}

namespace std
{
	template <>
	struct hash<state::CardRef>
	{
		std::size_t operator()(const state::CardRef& key) const
		{
			return std::hash<decltype(key.id)>()(key.id);
		}
	};
}
