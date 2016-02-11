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
	if (this->minion->stat.IsShield()) {
		this->minion->stat.SetShield(false);
	}
	else {
		this->minion->stat.SetHP(this->minion->stat.GetHP() - damage);

		this->HookMinionCheckEnraged();
	}
}

inline bool GameEngine::BoardObjects::MinionManipulator::IsForgetful() const
{
	return this->minion->stat.IsForgetful();
}

inline void GameEngine::BoardObjects::MinionManipulator::AttackedOnce()
{
	this->minion->attacked_times++;
	this->minion->stat.SetStealth(false);
}

inline void GameEngine::BoardObjects::MinionManipulator::SetFreezeAttacker(bool freeze)
{
	this->minion->stat.SetFreezeAttacker(freeze);
}

inline void GameEngine::BoardObjects::MinionManipulator::SetFreezed(bool freezed)
{
	this->minion->stat.SetFreezed(freezed);
}

inline bool GameEngine::BoardObjects::MinionManipulator::IsFreezeAttacker() const
{
	return this->minion->stat.IsFreezeAttacker();
}

inline bool GameEngine::BoardObjects::MinionManipulator::IsFreezed() const
{
	return this->minion->stat.IsFreezed();
}

inline void GameEngine::BoardObjects::MinionManipulator::AddAttack(int val) const
{
	this->minion->stat.SetAttack(this->minion->stat.GetAttack() + val);
}

inline void GameEngine::BoardObjects::MinionManipulator::IncreaseCurrentAndMaxHP(int val) const
{
#ifdef DEBUG
	if (val < 0) throw std::runtime_error("should we trigger heal? enrage effect? damaged effect? use TakeDamage() for that.");
#endif
	this->minion->stat.SetMaxHP(this->minion->stat.GetMaxHP() + val);
	this->minion->stat.SetHP(this->minion->stat.GetHP() + val);

	// no need to check enrage, since we add the hp and max-hp by the same amount
}

inline void GameEngine::BoardObjects::MinionManipulator::DecreaseMaxHP(int val) const
{
	this->minion->stat.SetMaxHP(this->minion->stat.GetMaxHP() - val);
	this->minion->stat.SetHP(std::min(this->minion->stat.GetHP(), this->minion->stat.GetMaxHP()));

	this->HookMinionCheckEnraged(); // might become un-enraged if max-hp lowered to current-hp
}

inline void GameEngine::BoardObjects::MinionManipulator::AddOnDeathTrigger(Minion::OnDeathTrigger func) const
{
	this->minion->triggers_on_death.push_back(func);
}

inline std::list<GameEngine::BoardObjects::Minion::OnDeathTrigger> GameEngine::BoardObjects::MinionManipulator::GetAndClearOnDeathTriggers() const
{
	std::list<GameEngine::BoardObjects::Minion::OnDeathTrigger> ret;
	this->minion->triggers_on_death.swap(ret);
	return ret;
}

inline void GameEngine::BoardObjects::MinionManipulator::SetMinionStatFlag(MinionStat::Flag flag, bool val) const
{
	this->minion->stat.SetFlag(flag, val);
}

inline void GameEngine::BoardObjects::MinionManipulator::AddAura(Aura * aura) const
{
	this->minion->auras.Add(*this, aura);
}

inline void GameEngine::BoardObjects::MinionManipulator::ClearAuras() const
{
	this->minion->auras.Clear(*this);
}

inline void GameEngine::BoardObjects::MinionManipulator::AddEnchantment(Enchantment * enchantment, EnchantmentOwner * owner) const
{
	this->minion->enchantments.Add(enchantment, owner, *this);
}

inline void GameEngine::BoardObjects::MinionManipulator::RemoveEnchantment(Enchantment * enchantment) const
{
	this->minion->enchantments.Remove(enchantment, *this);
}

inline void GameEngine::BoardObjects::MinionManipulator::ClearEnchantments() const
{
	this->minion->enchantments.Clear(*this);
}

inline void GameEngine::BoardObjects::MinionManipulator::HookAfterMinionAdded(MinionManipulator & added_minion) const
{
	this->minion->auras.HookAfterMinionAdded(*this, added_minion);
}

inline void GameEngine::BoardObjects::MinionManipulator::HookMinionCheckEnraged() const
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

inline void GameEngine::BoardObjects::MinionManipulator::TurnStart(bool owner_turn) const
{
	this->minion->summoned_this_turn = false;
	this->minion->attacked_times = 0;
}

inline void GameEngine::BoardObjects::MinionManipulator::TurnEnd(bool owner_turn) const
{
	if (owner_turn) {
		// check thaw
		if (this->minion->attacked_times == 0 && !this->minion->summoned_this_turn)
		{
			// if summon in this turn, and freeze it, then the minion will not be unfrozen
			this->minion->stat.SetFreezed(false);
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

	auto summoned_minion = GameEngine::BoardObjects::MinionManipulator(*this->board, *this->minions, *new_it);

	this->board->object_manager.HookAfterMinionAdded(summoned_minion);

	return summoned_minion;
}
