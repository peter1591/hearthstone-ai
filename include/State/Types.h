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
		kPlayerSecond
	};

	struct AnotherPlayer {
		PlayerIdentifier operator()(PlayerIdentifier player) {
			if (player == kPlayerFirst) return kPlayerSecond;
			else return kPlayerFirst;
		}
	};

	class CardRef
	{
	public:
		typedef typename Utils::CloneableContainers::Vector<Cards::Card>::Identifier IdentifierType;

		CardRef() {}
		explicit CardRef(IdentifierType id) : id(id) {}

		template <typename T> bool operator==(T&& rhs) const
		{
			static_assert(std::is_same<std::decay_t<T>, CardRef>::value, "Wrong type");
			return this->id == rhs.id;
		}

		template <typename T> bool operator!=(T&& rhs) const
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
			return Utils::CloneableContainers::VectorIdentifierHasher()(key.id);
		}
	};
}
