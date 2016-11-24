#pragma once

#include "State/Board/Deck.h"
#include "State/Board/Graveyard.h"
#include "State/Board/Hand.h"
#include "State/Board/Minions.h"
#include "State/Board/Secrets.h"
#include "State/Board/Weapon.h"
#include "State/Board/PlayerResource.h"

namespace State
{
	class Player
	{
	public:
		Player() : fatigue_damage_(0) {}

		CardRef hero_ref_;

		Deck deck_;
		Hand hand_;
		Minions minions_;
		Weapon weapon_;
		Secrets secrets_;
		Graveyard graveyard_;

		PlayerResource resource_;
		int fatigue_damage_;
	};
}