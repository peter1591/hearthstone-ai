#pragma once

#include <memory>
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
		friend class Minions; // only class 'Minions' can create a new instance

	public:
		MinionManipulator(Board & board, Minions & minions, Minion const& minion)
			: board(board), minions(minions), minion(minion)
		{}

		MinionManipulator(Board & board, Minions & minions, Minion && minion)
			: board(board), minions(minions), minion(std::move(minion))
		{}

		MinionManipulator(MinionManipulator const& rhs)
			: board(rhs.board), minions(rhs.minions), minion(rhs.minion)
		{}

		MinionManipulator(MinionManipulator && rhs)
			: board(rhs.board), minions(rhs.minions), minion(std::move(rhs.minion))
		{}

		MinionManipulator & operator=(MinionManipulator const& rhs) = delete;
		MinionManipulator & operator=(MinionManipulator && rhs) = delete;

		bool operator==(MinionManipulator const& rhs) const { return this->GetMinion() == rhs.GetMinion(); }
		bool operator!=(MinionManipulator const& rhs) const { return this->GetMinion() != rhs.GetMinion(); }

		Board & GetBoard() const { return this->board; }
		Minions & GetMinions() const { return this->minions; }
		Minion const& GetMinion() const { return this->minion; }

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
		void AddAttack(int val);
		void IncreaseCurrentAndMaxHP(int val);
		void DecreaseMaxHP(int val);

		void AddOnDeathTrigger(Minion::OnDeathTrigger func);
		std::list<Minion::OnDeathTrigger> GetAndClearOnDeathTriggers();

		void SetMinionStatFlag(MinionStat::Flag flag, bool val);

	public: // auras
		void AddAura(Aura * aura);
		void ClearAuras();

	public: // enchantments
		void AddEnchantment(Enchantment<MinionManipulator> * enchantment, EnchantmentOwner * owner);
		void RemoveEnchantment(Enchantment<MinionManipulator> * enchantment);
		void ClearEnchantments();

	public: // hooks
		void TurnStart(bool owner_turn);
		void TurnEnd(bool owner_turn);

	public: // triggering hooks
		void HookAfterMinionAdded(MinionManipulator & added_minion);
		void HookMinionCheckEnraged();

	public:
		bool IsPlayerSide() const;
		bool IsOpponentSide() const;

	private:
		Board & board;
		Minions & minions;
		Minion minion;
	};

} // BoardObjects
} // GameEngine