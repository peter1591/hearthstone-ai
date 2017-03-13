#pragma once

#include <assert.h>
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

	enum PlayerSide
	{
		kPlayerInvalid = 0,
		kPlayerFirst = 1,
		kPlayerSecond = 2,
	};

	class PlayerIdentifier
	{
	private:
		PlayerIdentifier(PlayerSide side) : side_(side) {}

	public:
		PlayerIdentifier() : side_(kPlayerInvalid) {}

		static PlayerIdentifier First() { return PlayerIdentifier(kPlayerFirst); }
		static PlayerIdentifier Second() { return PlayerIdentifier(kPlayerSecond); }

		bool operator==(PlayerIdentifier rhs) const { return side_ == rhs.side_; }
		bool operator!=(PlayerIdentifier rhs) const { return side_ != rhs.side_; }

		PlayerIdentifier Opposite() const
		{
			return PlayerIdentifier(OppositeSide());
		}

		void ChangeSide()
		{
			assert(AssertCheck());
			side_ = OppositeSide();
		}

		void SetFirst() { side_ = kPlayerFirst; }
		void SetSecond() { side_ = kPlayerSecond; }

		bool IsFirst() const
		{
			assert(AssertCheck());
			return side_ == kPlayerFirst;
		}

		bool IsSecond() const
		{
			assert(AssertCheck());
			return side_ == kPlayerSecond;
		}

		bool AssertCheck() const
		{
			return side_ == kPlayerFirst || side_ == kPlayerSecond;
		}

	private:
		PlayerSide OppositeSide() const
		{
			return (PlayerSide)(3 - side_);
		}

	private:
		PlayerSide side_;
	};

	class CardRef
	{
	public:
		typedef typename Utils::CloneableContainers::Vector<Cards::Card>::Identifier IdentifierType;

		CardRef() : id(IdentifierType::GetInvalidIdentifier()) {}
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
		std::size_t operator()(state::CardRef key) const
		{
			return std::hash<decltype(key.id)>()(key.id);
		}
	};
}
