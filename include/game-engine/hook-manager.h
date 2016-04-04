#pragma once

#include "game-engine/board-objects/minion.h"
#include "game-engine/board-objects/minions.h"

namespace GameEngine
{
	class Board;

	class HookManager
	{
	public:
		HookManager(Board & board);
		HookManager(HookManager const& rhs) = delete;
		HookManager(HookManager && rhs) = delete;
		HookManager & operator=(HookManager const& rhs) = delete;
		HookManager & operator=(HookManager && rhs) = delete;

	public: // hooks
		void PlayerTurnStart();
		void PlayerTurnEnd();
		void OpponentTurnStart();
		void OpponentTurnEnd();

		void HookAfterMinionAdded(Minion & minion);
		void HookAfterMinionDamaged(Minion & minion, int damage);
		void HookBeforeMinionTransform(Minion & minion, int new_card_id);
		void HookAfterMinionTransformed(Minion & minion);

	private:
		Board & board;
	};
}