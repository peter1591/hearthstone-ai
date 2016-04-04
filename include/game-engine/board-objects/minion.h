#pragma once

#include <memory>
#include "object-base.h"
#include "game-engine/board-objects/minion-data.h"
#include "game-engine/enchantments/enchantments.h"
#include "game-engine/hook/minion-auras.h"

namespace GameEngine {

	class Board;
	class Player;
	class Minions;

	class Minion : public ObjectBase
	{
		friend std::hash<Minion>;
		friend class Minions; // only class 'Minions' can create/copy/move

	public:
		typedef MinionData::OnDeathTrigger OnDeathTrigger;

	public:
		// move semantics should only be used by container (i.e., std::list<Minion>)
		// TODO: use unique_ptr in container? or try reference_wrapper
		Minion(Minion && rhs);

	private: // copy semantics should only be used in Minions
		Minion(Minion const& rhs) = delete;

		Minion(Minions & minions, Minion const& rhs);
		Minion(Minions & minions, Minion && minion);
		Minion(Minions & minions, MinionData const& minion);
		Minion(Minions & minions, MinionData && minion);

		Minion & operator=(Minion const& rhs) = delete;
		Minion & operator=(Minion && rhs) = delete;

	public:
		bool operator==(Minion const& rhs) const
		{
			if (this->GetMinion() != rhs.GetMinion()) return false;
			if (this->enchantments != rhs.enchantments) return false;
			if (this->auras != rhs.auras) return false;
			return true;
		}
		bool operator!=(Minion const& rhs) const { return this->GetMinion() != rhs.GetMinion(); }

		void Destroy();

		Board & GetBoard() const;
		Minions & GetMinions() const { return this->minions; }
		MinionData const& GetMinion() const { return this->minion; }

	public: // basic operations on minion (including interface to ObjectBase)
		int GetHP() const;
		int GetMaxHP() const;
		int GetAttack() const;

		bool Attackable() const;
		void AttackedOnce();

		void TakeDamage(int damage, bool poisonous);
		int GetForgetfulCount() const;

		void SetFreezed();
		bool IsFreezeAttacker() const;
		bool IsFreezed() const;

		bool IsPoisonous() const;

	public:
		void AddAttack(int val);
		void IncreaseCurrentAndMaxHP(int val);
		void DecreaseMaxHP(int val);
		void AddSpellDamage(int val);

		void AddOnDeathTrigger(OnDeathTrigger && func);
		std::list<OnDeathTrigger> GetAndClearOnDeathTriggers();

		void SetMinionStatFlag(MinionStat::Flag flag);
		void RemoveMinionStatFlag(MinionStat::Flag flag);
		void ClearMinionStatFlag(MinionStat::Flag flag);

		void Transform(MinionData const& minion);

	public: // hooks
		void TurnStart(bool owner_turn);
		void TurnEnd(bool owner_turn);

	public: // triggering hooks
		void HookAfterMinionAdded(Minion & added_minion);
		void HookMinionCheckEnraged();
		void HookAfterMinionDamaged(Minion & minion, int damage);
		void HookBeforeMinionTransform(Minion & minion, int new_card_id);
		void HookAfterMinionTransformed(Minion & minion);

	public:
		Player & GetPlayer() const;
		bool IsPlayerSide() const;
		bool IsOpponentSide() const;

	public:
		Enchantments<Minion> enchantments;
		MinionAuras auras;

	private:
		Minions & minions;
		MinionData minion;
	};

} // GameEngine

namespace std {
	template <> struct hash<GameEngine::Minion> {
		typedef GameEngine::Minion argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			result = std::hash<decltype(s.minion)>()(s.minion);
			GameEngine::hash_combine(result, s.enchantments);
			GameEngine::hash_combine(result, s.auras);

			return result;
		}
	};
}