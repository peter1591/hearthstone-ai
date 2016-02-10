#include "game-engine/board.h"
#include "game-engine/board-objects/minion.h"
#include "game-engine/board-objects/minion-manipulator.h"

inline void GameEngine::BoardObjects::MinionManipulator::AddAura(Aura * aura)
{
	this->minion->auras.Add(*this, aura);
}

inline void GameEngine::BoardObjects::MinionManipulator::ClearAuras()
{
	this->minion->auras.Clear(*this);
}

inline void GameEngine::BoardObjects::MinionManipulator::AddEnchantment(Enchantment * enchantment, EnchantmentOwner * owner)
{
	this->minion->enchantments.Add(enchantment, owner, *this);
}

inline void GameEngine::BoardObjects::MinionManipulator::RemoveEnchantment(Enchantment * enchantment)
{
	this->minion->enchantments.Remove(enchantment, *this);
}

inline void GameEngine::BoardObjects::MinionManipulator::ClearEnchantments()
{
	this->minion->enchantments.Clear(*this);
}

inline void GameEngine::BoardObjects::MinionManipulator::HookAfterMinionAdded(MinionManipulator & added_minion)
{
	this->minion->auras.HookAfterMinionAdded(*this, added_minion);
}

inline void GameEngine::BoardObjects::MinionManipulator::HookMinionCheckEnraged()
{
	auto & minion = *this->minion;
	if (minion.GetHP() < minion.GetMaxHP()) {
		minion.auras.HookAfterOwnerEnraged(*this); // enraged
	}
	else if (minion.GetHP() == minion.GetMaxHP()) {
		minion.auras.HookAfterOwnerUnEnraged(*this); // un-enraged
	}
	else {
		throw std::runtime_error("hp should not be larger than max-hp");
	}
}

inline void GameEngine::BoardObjects::MinionManipulator::TurnStart(bool owner_turn)
{
	this->minion->summoned_this_turn = false;
	this->minion->attacked_times = 0;
}

inline void GameEngine::BoardObjects::MinionManipulator::TurnEnd(bool owner_turn)
{
	if (owner_turn) {
		// check thaw
		if (this->minion->attacked_times == 0 && !this->minion->summoned_this_turn)
		{
			// if summon in this turn, and freeze it, then the minion will not be unfrozen
			this->minion->stat.SetFlag(MinionStat::FLAG_FREEZED, false);
		}
	}

	this->minion->enchantments.TurnEnd(*this);
}

inline bool GameEngine::BoardObjects::MinionManipulator::IsPlayerSide() const
{
	return this->board->object_manager.IsPlayerSide(this->minions);
}

inline bool GameEngine::BoardObjects::MinionManipulator::IsOpponentSide() const
{
	return this->board->object_manager.IsOpponentSide(this->minions);
}

inline bool GameEngine::BoardObjects::MinionInserter::IsEnd() const
{
	return this->it_minion == this->minions->minions.end();
}

inline GameEngine::BoardObjects::MinionInserter GameEngine::BoardObjects::MinionInserter::GetInserterBefore(MinionManipulator & minion)
{
	auto it = minion.minions->GetIterator(minion.minion);
	return MinionInserter(*minion.board, *minion.minions, it);
}

inline void GameEngine::BoardObjects::MinionInserter::GoToNext()
{
	this->it_minion++;
}

inline void GameEngine::BoardObjects::MinionInserter::EraseAndGoToNext()
{
	this->it_minion = this->minions->minions.erase(this->it_minion);
}

inline bool GameEngine::BoardObjects::MinionInserter::IsPendingRemoval() const
{
	return this->it_minion->pending_removal;
}

inline void GameEngine::BoardObjects::MinionInserter::MarkPendingRemoval()
{
	if (this->it_minion->pending_removal == false) return;
	this->it_minion->pending_removal = true;
	this->minions->pending_removal_count++;
}

inline GameEngine::BoardObjects::MinionManipulator GameEngine::BoardObjects::MinionInserter::ConverToManipulator()
{
#ifdef DEBUG
	if (this->it_minion->pending_removal) {
		throw std::runtime_error("Warning!!! trying to manipulate a pending-death minion!");
	}
#endif

	return MinionManipulator(*this->board, *this->minions, *this->it_minion);
}

inline GameEngine::BoardObjects::MinionManipulator GameEngine::BoardObjects::MinionInserter::InsertBefore(Minion && minion)
{
	auto new_it = this->minions->minions.insert(this->it_minion, std::move(minion));

	return GameEngine::BoardObjects::MinionManipulator(*this->board, *this->minions, *new_it);
}
