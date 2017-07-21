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
		kCardTypeInvalid,
		kCardTypeHero,
		kCardTypeMinion,
		kCardTypeSpell,
		kCardTypeWeapon,
		kCardTypeHeroPower,
		kCardTypeEnchantment
	};

	enum CardZone
	{
		kCardZoneInvalid,
		kCardZonePlay,
		kCardZoneHand,
		kCardZoneGraveyard,
		kCardZoneRemoved,
		kCardZoneSetASide,
		kCardZoneNewlyCreated // ready to be pushed to cards manager (only for debug assertion)
	};

	enum CardRace
	{
		kCardRaceInvalid,
		kCardRaceDemon,
		kCardRaceMech,
		kCardRaceDragon,
		kCardRaceBeast,
		kCardRacePirate,
		kCardRaceMurloc,
		kCardRaceTotem,
		kCardRaceElemental,
		kCardRaceOrc
	};

	enum CardSet
	{
		kCardSetInvalid,
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
		kCardSetUngoro,
		kCardSetHOF, // hero of fame
		kCardSetTB
	};

	enum CardRarity
	{
		kCardRarityInvalid,
		kCardRarityCommon,
		kCardRarityRare,
		kCardRarityEpic,
		kCardRarityLegendary
	};

	enum PlayerSide
	{
		kPlayerInvalid = 0,
		kPlayerFirst = 1,
		kPlayerSecond = 2,
	};

	template <PlayerSide Side> struct ValidPlayerSide {
		static constexpr bool valid = false;
	};

	template <> struct ValidPlayerSide<kPlayerFirst> {
		static constexpr bool valid = true;
	};
	template <> struct ValidPlayerSide<kPlayerSecond> {
		static constexpr bool valid = true;
	};


	class PlayerIdentifier
	{
	public:
		PlayerIdentifier() : side_(kPlayerInvalid) {}
		constexpr PlayerIdentifier(PlayerSide side) : side_(side) {}

		constexpr static PlayerIdentifier First() { return PlayerIdentifier(kPlayerFirst); }
		constexpr static PlayerIdentifier Second() { return PlayerIdentifier(kPlayerSecond); }

		bool operator==(PlayerIdentifier rhs) const { return side_ == rhs.side_; }
		bool operator!=(PlayerIdentifier rhs) const { return side_ != rhs.side_; }

		bool IsValid() const { return side_ != kPlayerInvalid; }
		void InValidate() { side_ = kPlayerInvalid; }

		PlayerIdentifier Opposite() const
		{
			return PlayerIdentifier(OppositeSide());
		}

		void ChangeSide()
		{
			assert(AssertCheck());
			side_ = OppositeSide();
		}

		PlayerSide GetSide() const { return side_; }
		void SetSide(PlayerSide side) { side_ = side; }

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
			assert(side_ == kPlayerFirst || side_ == kPlayerSecond);
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
