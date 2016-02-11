#include "game-engine/board.h"
#include "game-engine/board-objects/minion.h"
#include "game-engine/board-objects/minion-manipulator.h"

inline int GameEngine::BoardObjects::MinionManipulator::GetHP() const
{
	return this->minion->stat.GetHP();
}

inline int GameEngine::BoardObjects::MinionManipulator::GetMaxHP() const
{
	return this->minion->stat.GetMaxHP();
}

inline int GameEngine::BoardObjects::MinionManipulator::GetAttack() const
{
	return this->minion->stat.GetAttack();
}

inline void GameEngine::BoardObjects::MinionManipulator::TakeDamage(int damage)
{
	if (this->minion->stat.GetFlag(MinionStat::FLAG_SHIELD)) {
		this->minion->stat.SetFlag(MinionStat::FLAG_SHIELD, false);
	}
	else {
		this->minion->stat.SetHP(this->minion->stat.GetHP() - damage);
	}
}

inline bool GameEngine::BoardObjects::MinionManipulator::IsForgetful() const
{
	return this->minion->stat.GetFlag(MinionStat::FLAG_FORGETFUL);
}

inline void GameEngine::BoardObjects::MinionManipulator::AttackedOnce()
{
	this->minion->attacked_times++;
	this->minion->stat.SetFlag(MinionStat::FLAG_STEALTH, false);
}

inline void GameEngine::BoardObjects::MinionManipulator::SetFreezeAttacker(bool freeze)
{
	this->minion->stat.SetFlag(MinionStat::FLAG_FREEZE_ATTACKER, freeze);
}

inline void GameEngine::BoardObjects::MinionManipulator::SetFreezed(bool freezed)
{
	this->minion->stat.SetFlag(MinionStat::FLAG_FREEZED, freezed);
}

inline bool GameEngine::BoardObjects::MinionManipulator::IsFreezeAttacker() const
{
	return this->minion->stat.GetFlag(MinionStat::FLAG_FREEZE_ATTACKER);
}

inline bool GameEngine::BoardObjects::MinionManipulator::IsFreezed() const
{
	return this->minion->stat.GetFlag(MinionStat::FLAG_FREEZED);
}

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
	if (this->GetHP() < this->GetMaxHP()) {
		minion.auras.HookAfterOwnerEnraged(*this); // enraged
	}
	else if (this->GetHP() == this->GetMaxHP()) {
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
	if (this->it_minion->pending_removal) this->minions->pending_removal_count--;
	this->it_minion = this->minions->minions.erase(this->it_minion);
}

inline bool GameEngine::BoardObjects::MinionInserter::IsPendingRemoval() const
{
	return this->it_minion->pending_removal;
}

inline void GameEngine::BoardObjects::MinionInserter::MarkPendingRemoval()
{
	if (this->it_minion->pending_removal) return;
	this->it_minion->pending_removal = true;
	this->minions->pending_removal_count++;
}

inline GameEngine::BoardObjects::MinionManipulator GameEngine::BoardObjects::MinionInserter::ConverToManipulator()
{
	return MinionManipulator(*this->board, *this->minions, *this->it_minion);
}

inline GameEngine::BoardObjects::MinionManipulator GameEngine::BoardObjects::MinionInserter::InsertBefore(Minion && minion)
{
	auto new_it = this->minions->minions.insert(this->it_minion, std::move(minion));

	return GameEngine::BoardObjects::MinionManipulator(*this->board, *this->minions, *new_it);
}
