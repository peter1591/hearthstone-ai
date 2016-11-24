#pragma once

namespace State
{
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
		kCardZonePutASide,
		kCardZoneInvalid
	};
}