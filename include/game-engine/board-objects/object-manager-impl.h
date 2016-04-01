#pragma once

#include "game-engine/board.h"
#include "object-manager.h"

namespace GameEngine
{
	namespace BoardObjects {

		inline void ObjectManager::SetHero(HeroData const & player, HeroData const & opponent)
		{
			this->board.player.hero.SetHero(player);
			this->opponent_hero.SetHero(opponent);
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
				return BoardObject(this->opponent_hero);
			else if (idx < SLOT_MAX)
				return BoardObject(this->GetMinion(idx));
			else
				throw std::runtime_error("invalid argument");
		}

		inline ObjectManager::ObjectManager(Board & board) :
			board(board), opponent_hero(board),
			player_minions(board), opponent_minions(board)
		{
		}

		inline ObjectManager::ObjectManager(Board & board, ObjectManager const & rhs) :
			board(board), opponent_hero(board, std::move(rhs.opponent_hero)),
			player_minions(board), opponent_minions(board)
		{
			this->player_minions = rhs.player_minions;
			this->opponent_minions = rhs.opponent_minions;
		}

		inline ObjectManager::ObjectManager(Board & board, ObjectManager && rhs) :
			board(board), opponent_hero(board),
			player_minions(board), opponent_minions(board)
		{
			*this = std::move(rhs);
		}

		inline ObjectManager & ObjectManager::operator=(ObjectManager && rhs)
		{
			this->opponent_hero = std::move(rhs.opponent_hero);
			this->player_minions = std::move(rhs.player_minions);
			this->opponent_minions = std::move(rhs.opponent_minions);

			return *this;
		}

		inline bool ObjectManager::operator==(ObjectManager const & rhs) const
		{
			if (this->opponent_hero != rhs.opponent_hero) return false;

			if (this->player_minions != rhs.player_minions) return false;
			if (this->opponent_minions != rhs.opponent_minions) return false;

			return true;
		}

		inline bool ObjectManager::operator!=(ObjectManager const & rhs) const
		{
			return !(*this == rhs);
		}

		inline Hero & ObjectManager::GetHeroBySide(SlotIndex side)
		{
			if (side == SLOT_PLAYER_SIDE) return this->board.player.hero;
			else if (side == SLOT_OPPONENT_SIDE) return this->opponent_hero;
			else throw std::runtime_error("invalid argument");
		}

		inline MinionIterator ObjectManager::GetMinionIterator(SlotIndex slot_idx)
		{
			if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx < SLOT_OPPONENT_HERO) {
				return this->player_minions.GetIterator(slot_idx - SLOT_PLAYER_MINION_START);
			}
			else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx < SLOT_MAX) {
				return this->opponent_minions.GetIterator(slot_idx - SLOT_OPPONENT_MINION_START);
			}
			else throw std::runtime_error("invalid argument");
		}

		inline Minion & ObjectManager::GetMinion(SlotIndex slot_idx)
		{
			if (slot_idx < SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx == SLOT_PLAYER_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx < SLOT_OPPONENT_HERO) {
				return this->player_minions.GetMinion(slot_idx - SLOT_PLAYER_MINION_START);
			}
			else if (slot_idx == SLOT_OPPONENT_HERO) throw std::runtime_error("invalid argument");
			else if (slot_idx < SLOT_MAX) {
				return this->opponent_minions.GetMinion(slot_idx - SLOT_OPPONENT_MINION_START);
			}
			else throw std::runtime_error("invalid argument");
		}

		inline void ObjectManager::PlayerTurnStart()
		{
			this->board.player.hero.TurnStart(true);
			this->opponent_hero.TurnStart(false);
			this->player_minions.TurnStart(true);
			this->opponent_minions.TurnStart(false);
		}

		inline void ObjectManager::PlayerTurnEnd()
		{
			this->board.player.hero.TurnEnd(true);
			this->opponent_hero.TurnEnd(false);
			this->player_minions.TurnEnd(true);
			this->opponent_minions.TurnEnd(false);
		}

		inline void ObjectManager::OpponentTurnStart()
		{
			this->opponent_hero.TurnStart(true);
			this->board.player.hero.TurnStart(false);
			this->opponent_minions.TurnStart(true);
			this->player_minions.TurnStart(false);
		}

		inline void ObjectManager::OpponentTurnEnd()
		{
			this->opponent_hero.TurnEnd(true);
			this->board.player.hero.TurnEnd(false);
			this->opponent_minions.TurnEnd(true);
			this->player_minions.TurnEnd(false);
		}

		inline void ObjectManager::HookAfterMinionAdded(Minion & added_minion)
		{
			this->board.player.hero.HookAfterMinionAdded(added_minion);
			this->opponent_hero.HookAfterMinionAdded(added_minion);
			this->player_minions.HookAfterMinionAdded(added_minion);
			this->opponent_minions.HookAfterMinionAdded(added_minion);
		}

		inline void ObjectManager::HookAfterMinionDamaged(Minions & minions, Minion & minion, int damage)
		{
			this->board.player.hero.HookAfterMinionDamaged(minions, minion, damage);
			this->opponent_hero.HookAfterMinionDamaged(minions, minion, damage);
			this->player_minions.HookAfterMinionDamaged(minions, minion, damage);
			this->opponent_minions.HookAfterMinionDamaged(minions, minion, damage);
		}

		inline void ObjectManager::DebugPrint() const
		{
			std::cout << "Player Hero: " << this->board.player.hero.GetDebugString() << std::endl;
			std::cout << "Opponent Hero: " << this->opponent_hero.GetDebugString() << std::endl;

			std::cout << "Opponent minions: " << std::endl;
			this->opponent_minions.DebugPrint();

			std::cout << "Player minions: " << std::endl;
			this->player_minions.DebugPrint();
		}

	}
}