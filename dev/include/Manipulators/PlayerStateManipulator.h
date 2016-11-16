#pragma once

#include <algorithm>
#include "EventManager/HandlersManager.h"
#include "State/PlayerState.h"

namespace Manipulators
{
	class PlayerStateManipulator
	{
	public:
		PlayerStateManipulator(State::PlayerState & player_state, EventManager::HandlersManager & event_mgr) : 
			player_state_(player_state), event_mgr_(event_mgr)
		{
		}

		void TakeDamage(int damage)
		{
			if (damage < 0) return;

			if (player_state_.shields_ > 0)
			{
				if (damage <= player_state_.shields_)
				{
					player_state_.shields_ -= damage;
					return;
				}

				damage -= player_state_.shields_;
				player_state_.shields_ = 0;
			}

			player_state_.hp_ -= damage;

			// TODO: trigger event - player take damage
		}

		void SetHP(int hp)
		{
			player_state_.hp_ = hp;
		}

	private:
		State::PlayerState & player_state_;
		EventManager::HandlersManager & event_mgr_;
	};
}