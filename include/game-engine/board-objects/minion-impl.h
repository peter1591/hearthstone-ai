#include "game-engine/board.h"
#include "game-engine/board-objects/minion-data.h"
#include "game-engine/board-objects/minion.h"

inline GameEngine::Board & GameEngine::BoardObjects::Minion::GetBoard() const
{
	return this->minions.GetBoard();
}

inline int GameEngine::BoardObjects::Minion::GetHP() const
{
	return this->minion.stat.GetHP();
}

inline int GameEngine::BoardObjects::Minion::GetMaxHP() const
{
	return this->minion.stat.GetMaxHP();
}

inline int GameEngine::BoardObjects::Minion::GetAttack() const
{
	return this->minion.stat.GetAttack();
}

inline void GameEngine::BoardObjects::Minion::TakeDamage(int damage, bool poisonous)
{
	if (this->minion.stat.IsShield()) {
		this->minion.stat.ClearShield();
	}
	else {
		this->minion.stat.SetHP(this->minion.stat.GetHP() - damage);

		if (poisonous) {
			this->GetMinions().MarkPendingRemoval(*this);
		}

		this->HookMinionCheckEnraged();
		this->GetBoard().object_manager.HookAfterMinionDamaged(this->minions, *this, damage);
	}
}

inline bool GameEngine::BoardObjects::Minion::IsForgetful() const
{
	return this->minion.stat.IsForgetful();
}

inline bool GameEngine::BoardObjects::Minion::Attackable() const
{
	if (this->minion.summoned_this_turn && !this->minion.stat.IsCharge()) return false;

	if (this->minion.stat.IsFreezed()) return false;

	int max_attacked_times = 1;
	if (this->minion.stat.IsWindFury()) max_attacked_times = 2;

	if (this->minion.attacked_times >= max_attacked_times) return false;

	if (this->minion.stat.GetAttack() <= 0) return false;

	return true;
}

inline void GameEngine::BoardObjects::Minion::AttackedOnce()
{
	this->minion.attacked_times++;
	if (this->minion.stat.IsStealth()) this->minion.stat.ClearStealth();
}

inline void GameEngine::BoardObjects::Minion::SetFreezed()
{
	this->minion.stat.SetFreezed();
}

inline bool GameEngine::BoardObjects::Minion::IsFreezeAttacker() const
{
	return this->minion.stat.IsFreezeAttacker();
}

inline bool GameEngine::BoardObjects::Minion::IsFreezed() const
{
	return this->minion.stat.IsFreezed();
}

inline bool GameEngine::BoardObjects::Minion::IsPoisonous() const
{
	return this->minion.stat.IsPoisonous();
}

inline void GameEngine::BoardObjects::Minion::AddAttack(int val)
{
	this->minion.stat.SetAttack(this->minion.stat.GetAttack() + val);
}

inline void GameEngine::BoardObjects::Minion::IncreaseCurrentAndMaxHP(int val)
{
#ifdef DEBUG
	if (val < 0) throw std::runtime_error("should we trigger heal? enrage effect? damaged effect? use TakeDamage() for that.");
#endif
	this->minion.stat.SetMaxHP(this->minion.stat.GetMaxHP() + val);
	this->minion.stat.SetHP(this->minion.stat.GetHP() + val);

	// no need to check enrage, since we add the hp and max-hp by the same amount
}

inline void GameEngine::BoardObjects::Minion::DecreaseMaxHP(int val)
{
	this->minion.stat.SetMaxHP(this->minion.stat.GetMaxHP() - val);
	this->minion.stat.SetHP(std::min(this->minion.stat.GetHP(), this->minion.stat.GetMaxHP()));

	this->HookMinionCheckEnraged(); // might become un-enraged if max-hp lowered to current-hp
}

inline void GameEngine::BoardObjects::Minion::AddOnDeathTrigger(OnDeathTrigger && func)
{
	this->minion.triggers_on_death.push_back(std::move(func));
}

inline std::list<GameEngine::BoardObjects::Minion::OnDeathTrigger> GameEngine::BoardObjects::Minion::GetAndClearOnDeathTriggers()
{
	std::list<GameEngine::BoardObjects::Minion::OnDeathTrigger> ret;
	this->minion.triggers_on_death.swap(ret);
	return ret;
}

inline void GameEngine::BoardObjects::Minion::SetMinionStatFlag(MinionStat::Flag flag)
{
	this->minion.stat.SetFlag(flag);
}

inline void GameEngine::BoardObjects::Minion::RemoveMinionStatFlag(MinionStat::Flag flag)
{
	this->minion.stat.RemoveFlag(flag);
}

inline void GameEngine::BoardObjects::Minion::ClearMinionStatFlag(MinionStat::Flag flag)
{
	this->minion.stat.ClearFlag(flag);
}

inline void GameEngine::BoardObjects::Minion::AddAura(std::unique_ptr<Aura> && aura)
{
	this->minion.auras.Add(*this, std::move(aura));
}

inline void GameEngine::BoardObjects::Minion::ClearAuras()
{
	this->minion.auras.Clear(*this);
}

inline void GameEngine::BoardObjects::Minion::AddEnchantment(
	std::unique_ptr<Enchantment<Minion>> && enchantment, EnchantmentOwner * owner)
{
	this->minion.enchantments.Add(std::move(enchantment), owner, *this);
}

inline void GameEngine::BoardObjects::Minion::RemoveEnchantment(Enchantment<Minion> * enchantment)
{
	this->minion.enchantments.Remove(enchantment, *this);
}

inline void GameEngine::BoardObjects::Minion::ClearEnchantments()
{
	this->minion.enchantments.Clear(*this);
}

inline void GameEngine::BoardObjects::Minion::HookAfterMinionAdded(Minion & added_minion)
{
	this->minion.auras.HookAfterMinionAdded(*this, added_minion);
}

inline void GameEngine::BoardObjects::Minion::HookMinionCheckEnraged()
{
	auto & minion = this->minion;
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

inline void GameEngine::BoardObjects::Minion::HookAfterMinionDamaged(Minions & minions, Minion & minion, int damage)
{
	this->minion.auras.HookAfterMinionDamaged(minions, minion, damage);
}

inline void GameEngine::BoardObjects::Minion::TurnStart(bool owner_turn)
{
	this->minion.summoned_this_turn = false;
	this->minion.attacked_times = 0;
}

inline void GameEngine::BoardObjects::Minion::TurnEnd(bool owner_turn)
{
	if (owner_turn) {
		// check thaw
		// Note: if summon in this turn, and freeze it, then the minion will not be unfrozen
		if (this->minion.attacked_times == 0 && !this->minion.summoned_this_turn) {
			if (this->IsFreezed()) this->minion.stat.ClearFreezed();
		}
	}

	this->minion.enchantments.TurnEnd(*this);
}

inline bool GameEngine::BoardObjects::Minion::IsPlayerSide() const
{
	return &this->GetBoard().object_manager.player_minions == &this->minions;
}

inline bool GameEngine::BoardObjects::Minion::IsOpponentSide() const
{
	return &this->GetBoard().object_manager.opponent_minions == &this->minions;
}