#pragma once

#include "state/board/Deck.h"
#include "state/board/Graveyard.h"
#include "state/board/Hand.h"
#include "state/board/Minions.h"
#include "state/board/Secrets.h"
#include "state/board/PlayerResource.h"

namespace state
{
	namespace board
	{
		class Player
		{
		public:
			Player() : fatigue_damage_(0) {}

			int GetFatigueDamage() const { return fatigue_damage_; }
			void SetFatigueDamage(int v) { fatigue_damage_ = v; }
			void IncreaseFatigueDamage() { ++fatigue_damage_; }

			PlayerResource const& GetResource() const { return resource_; }
			PlayerResource & GetResource() { return resource_; }

		public:
			CardRef hero_ref_;

			Deck deck_;
			Hand hand_;
			Minions minions_;
			Secrets secrets_;
			Graveyard graveyard_;


		private:
			PlayerResource resource_;
			int fatigue_damage_;
		};
	}
}