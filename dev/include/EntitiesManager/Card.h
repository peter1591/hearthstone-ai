#pragma once

#include <string>

enum CardType
{
	kCardTypeMinion,
	kCardTypeSpell,
	kCardTypeWeapon,
	kCardTypeSecret,
	kCardTypeHeroPower,
	kCardTypeInvalid
};

class Card
{
public:
	CardType card_type;
	std::string card_id;
};