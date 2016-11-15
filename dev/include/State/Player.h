#pragma once

#include "State/Deck.h"
#include "State/Graveyard.h"
#include "State/Hand.h"
#include "State/Minions.h"
#include "State/Secrets.h"
#include "State/Weapon.h"
#include "State/PlayerResource.h"

namespace State
{
	class Player
	{
	public:
		Deck deck_;
		Hand hand_;
		Minions minions_;
		Weapon weapon_;
		Secrets secrets_;
		Graveyard graveyard_;

		PlayerResource resource_;
	};
}