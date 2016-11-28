#pragma once

#include "State/board/Deck.h"
#include "State/board/Graveyard.h"
#include "State/board/Hand.h"
#include "State/board/Minions.h"
#include "State/board/Secrets.h"
#include "State/board/Weapon.h"
#include "State/board/PlayerResource.h"

namespace state
{
	namespace board
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
}