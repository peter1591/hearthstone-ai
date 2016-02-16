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

		void TakeDamage(int damage, bool poisonous) const;
		bool IsForgetful() const;

		void AttackedOnce() const;

		void SetFreezeAttacker(bool freeze) const;
		void SetFreezed(bool freezed) const;
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

	// Inserter can be used to insert minion into minions, and
	// it can be used as a iterator to iterates all the minions
	class MinionInserter
	{
	private:
		typedef std::list<Minion> minions_container_type;
		typedef minions_container_type::iterator minions_iterator;
		typedef minions_container_type::const_iterator minions_const_iterator;

	public:
		MinionInserter(Board & board, Minions & minions, minions_iterator it_minion)
			: board(&board), minions(&minions), it_minion(it_minion)
		{}

		Board & GetBoard() const { return *this->board; }
		Minions & GetMinions() const { return *this->minions; }

		MinionManipulator ConverToManipulator();

	public:
		bool IsEnd() const;

		void GoToNext();
		void EraseAndGoToNext();

		bool IsPendingRemoval() const;
		void MarkPendingRemoval();

		MinionManipulator InsertBefore(Minion && minion);

	private:
		Board * const board;
		Minions * const minions;
		minions_iterator it_minion;
	};

} // BoardObjects
} // GameEngine