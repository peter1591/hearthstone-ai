#pragma once

#include "object-base.h"
#include "game-engine/board-objects/minion.h"

namespace GameEngine {

class Board;

namespace BoardObjects {

	class Minions;

	// Manipulator is designed to modify data members for Minion structure,
	// and will trigger related hooks on-the-fly
	class MinionManipulator : public ObjectBase
	{
	public:
		MinionManipulator(Board & board, Minions & minions, Minion & minion)
			: board(&board), minions(&minions), minion(&minion)
		{}

		Board & GetBoard() const { return *this->board; }
		Minions & GetMinions() const { return *this->minions; }
		Minion const& GetMinion() const { return *this->minion; }

	public: // basic operations on minion (including interface to ObjectBase)
		int GetHP() const;
		int GetMaxHP() const;
		int GetAttack() const;

		void TakeDamage(int damage, bool poisonous);
		bool IsForgetful() const;

		void AttackedOnce();

		void SetFreezeAttacker(bool freeze);
		void SetFreezed(bool freezed);
		bool IsFreezeAttacker() const;
		bool IsFreezed() const;

		bool IsPoisonous() const;

	public:
		void AddAttack(int val) const;
		void IncreaseCurrentAndMaxHP(int val) const;
		void DecreaseMaxHP(int val) const;

		void AddOnDeathTrigger(Minion::OnDeathTrigger func) const;
		std::list<Minion::OnDeathTrigger> GetAndClearOnDeathTriggers() const;

		void SetMinionStatFlag(MinionStat::Flag flag, bool val) const;

		bool IsPendingRemoval() const;
		void MarkPendingRemoval() const;

	public: // auras
		void AddAura(Aura * aura) const;
		void ClearAuras() const;

	public: // enchantments
		void AddEnchantment(Enchantment<MinionManipulator> * enchantment, EnchantmentOwner * owner) const;
		void RemoveEnchantment(Enchantment<MinionManipulator> * enchantment) const;
		void ClearEnchantments() const;

	public: // hooks
		void TurnStart(bool owner_turn) const;
		void TurnEnd(bool owner_turn) const;

	public: // triggering hooks
		void HookAfterMinionAdded(MinionManipulator & added_minion) const;
		void HookMinionCheckEnraged() const;

	public:
		bool IsPlayerSide() const;
		bool IsOpponentSide() const;

	private:
		Board * const board;
		Minions * const minions;
		Minion * const minion;
	};

} // BoardObjects
} // GameEngine