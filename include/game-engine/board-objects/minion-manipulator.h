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

	public: // basic operations on minion (including interface to ObjectBase)
		int GetHP() const;
		int GetMaxHP() const;
		int GetAttack() const;

		void TakeDamage(int damage);
		bool IsForgetful() const;

		void AttackedOnce();

		void SetFreezeAttacker(bool freeze);
		void SetFreezed(bool freezed);
		bool IsFreezeAttacker() const;
		bool IsFreezed() const;

	public: // auras
		void AddAura(Aura * aura);
		void ClearAuras();

	public: // enchantments
		void AddEnchantment(Enchantment * enchantment, EnchantmentOwner * owner);
		void RemoveEnchantment(Enchantment * enchantment);
		void ClearEnchantments();

	public: // hooks
		void HookAfterMinionAdded(MinionManipulator & added_minion);
		void HookMinionCheckEnraged();

		void TurnStart(bool owner_turn);
		void TurnEnd(bool owner_turn);

	public:
		Board * const board;
		Minions * const minions;
		Minion * const minion;

	public:
		bool IsPlayerSide() const;
		bool IsOpponentSide() const;
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

		static MinionInserter GetInserterBefore(MinionManipulator & minion);
		MinionManipulator ConverToManipulator();

	public:
		bool IsEnd() const;

		void GoToNext();
		void EraseAndGoToNext();

		bool IsPendingRemoval() const;
		void MarkPendingRemoval();

		MinionManipulator InsertBefore(Minion && minion);

	public:
		Board * const board;
		Minions * const minions;
		minions_iterator it_minion;
	};

} // BoardObjects
} // GameEngine