#pragma once

#include "board.h"
#include "hook-manager.h"

namespace GameEngine
{
	inline HookManager::HookManager(Board & board) : board(board)
	{

	}

	inline void HookManager::PlayerTurnStart()
	{
		this->board.player.hero.TurnStart(true);
		this->board.opponent.hero.TurnStart(false);
		this->board.player.minions.TurnStart(true);
		this->board.opponent.minions.TurnStart(false);
	}

	inline void HookManager::PlayerTurnEnd()
	{
		this->board.player.hero.TurnEnd(true);
		this->board.opponent.hero.TurnEnd(false);
		this->board.player.minions.TurnEnd(true);
		this->board.opponent.minions.TurnEnd(false);
	}

	inline void HookManager::OpponentTurnStart()
	{
		this->board.opponent.hero.TurnStart(true);
		this->board.player.hero.TurnStart(false);
		this->board.opponent.minions.TurnStart(true);
		this->board.player.minions.TurnStart(false);
	}

	inline void HookManager::OpponentTurnEnd()
	{
		this->board.opponent.hero.TurnEnd(true);
		this->board.player.hero.TurnEnd(false);
		this->board.opponent.minions.TurnEnd(true);
		this->board.player.minions.TurnEnd(false);
	}

	inline void HookManager::HookAfterMinionAdded(BoardObjects::Minion & added_minion)
	{
		this->board.player.hero.HookAfterMinionAdded(added_minion);
		this->board.opponent.hero.HookAfterMinionAdded(added_minion);
		this->board.player.minions.HookAfterMinionAdded(added_minion);
		this->board.opponent.minions.HookAfterMinionAdded(added_minion);
	}

	inline void HookManager::HookAfterMinionDamaged(BoardObjects::Minion & minion, int damage)
	{
		this->board.player.hero.HookAfterMinionDamaged(minion, damage);
		this->board.opponent.hero.HookAfterMinionDamaged(minion, damage);
		this->board.player.minions.HookAfterMinionDamaged(minion, damage);
		this->board.opponent.minions.HookAfterMinionDamaged(minion, damage);
	}
}