#pragma once

#include "game-engine/board.h"
#include "object-manager.h"

namespace GameEngine
{
	namespace BoardObjects {

		inline void ObjectManager::SetHero(HeroData const & player, HeroData const & opponent)
		{
			this->board.player.hero.SetHero(player);
			this->board.opponent.hero.SetHero(opponent);
		}

		inline BoardObject ObjectManager::GetObject(SlotIndex idx)
		{
			if (idx < SLOT_PLAYER_HERO)
				throw std::runtime_error("invalid argument");
			else if (idx == SLOT_PLAYER_HERO)
				return BoardObject(this->board.player.hero);
			else if (idx < SLOT_OPPONENT_HERO)
				return BoardObject(this->GetMinion(idx));
			else if (idx == SLOT_OPPONENT_HERO)
				return BoardObject(this->board.opponent.hero);
			else if (idx < SLOT_MAX)
				return BoardObject(this->GetMinion(idx));
			else
				throw std::runtime_error("invalid argument");
		}

		inline ObjectManager::ObjectManager(Board & board) :
			board(board)
		{
		}

		inline Hero & ObjectManager::GetHeroBySide(SlotIndex side)
		{
			if (side == SLOT_PLAYER_SIDE) return this->board.player.hero;
			else if (side == SLOT_OPPONENT_SIDE) return this->board.opponent.hero;
			else throw std::runtime_error("invalid argument");
		}

		inline MinionConstIteratorWithSlotIndex ObjectManager::GetMinionsIteratorWithIndexAtBeginOfSide(SlotIndex side) const
		{
			if (side == SLOT_PLAYER_SIDE) {
				return this->board.player.minions.GetMinionsIteratorWithIndexAtBegin(SLOT_PLAYER_MINION_START);
			}
			else if (side == SLOT_OPPONENT_SIDE) {
				return this->board.opponent.minions.GetMinionsIteratorWithIndexAtBegin(SLOT_OPPONENT_MINION_START);
			}
			else throw std::runtime_error("invalid argument");
		}

		inline MinionIterator ObjectManager::GetMinionIterator(SlotIndex slot_idx)
		{
			if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx < SLOT_OPPONENT_HERO) {
				return this->board.player.minions.GetIterator(slot_idx - SLOT_PLAYER_MINION_START);
			}
			else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx < SLOT_MAX) {
				return this->board.opponent.minions.GetIterator(slot_idx - SLOT_OPPONENT_MINION_START);
			}
			else throw std::runtime_error("invalid argument");
		}

		inline MinionIterator ObjectManager::GetMinionIteratorAtBeginOfSide(SlotIndex side)
		{
			if (side == SLOT_PLAYER_SIDE) return this->GetMinionIterator(SLOT_PLAYER_MINION_START);
			else if (side == SLOT_OPPONENT_SIDE) return this->GetMinionIterator(SLOT_OPPONENT_MINION_START);
			else throw std::runtime_error("invalid argument");
		}

		inline Minion & ObjectManager::GetMinion(SlotIndex slot_idx)
		{
			if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx < SLOT_OPPONENT_HERO) {
				return this->board.player.minions.GetMinion(slot_idx - SLOT_PLAYER_MINION_START);
			}
			else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx < SLOT_MAX) {
				return this->board.opponent.minions.GetMinion(slot_idx - SLOT_OPPONENT_MINION_START);
			}
			else throw std::runtime_error("invalid argument");
		}

		inline void ObjectManager::PlayerTurnStart()
		{
			this->board.player.hero.TurnStart(true);
			this->board.opponent.hero.TurnStart(false);
			this->board.player.minions.TurnStart(true);
			this->board.opponent.minions.TurnStart(false);
		}

		inline void ObjectManager::PlayerTurnEnd()
		{
			this->board.player.hero.TurnEnd(true);
			this->board.opponent.hero.TurnEnd(false);
			this->board.player.minions.TurnEnd(true);
			this->board.opponent.minions.TurnEnd(false);
		}

		inline void ObjectManager::OpponentTurnStart()
		{
			this->board.opponent.hero.TurnStart(true);
			this->board.player.hero.TurnStart(false);
			this->board.opponent.minions.TurnStart(true);
			this->board.player.minions.TurnStart(false);
		}

		inline void ObjectManager::OpponentTurnEnd()
		{
			this->board.opponent.hero.TurnEnd(true);
			this->board.player.hero.TurnEnd(false);
			this->board.opponent.minions.TurnEnd(true);
			this->board.player.minions.TurnEnd(false);
		}

		inline void ObjectManager::HookAfterMinionAdded(Minion & added_minion)
		{
			this->board.player.hero.HookAfterMinionAdded(added_minion);
			this->board.opponent.hero.HookAfterMinionAdded(added_minion);
			this->board.player.minions.HookAfterMinionAdded(added_minion);
			this->board.opponent.minions.HookAfterMinionAdded(added_minion);
		}

		inline void ObjectManager::HookAfterMinionDamaged(Minions & minions, Minion & minion, int damage)
		{
			this->board.player.hero.HookAfterMinionDamaged(minions, minion, damage);
			this->board.opponent.hero.HookAfterMinionDamaged(minions, minion, damage);
			this->board.player.minions.HookAfterMinionDamaged(minions, minion, damage);
			this->board.opponent.minions.HookAfterMinionDamaged(minions, minion, damage);
		}
	}
}