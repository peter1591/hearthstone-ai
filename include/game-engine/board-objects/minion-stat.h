#pragma once

#include <array>
#include "game-engine/common.h"

namespace GameEngine {
class MinionStat
{
	friend std::hash<MinionStat>;

public:
	enum Flag {
		FLAG_TAUNT = 0,
		FLAG_CHARGE,
		FLAG_SHIELD,
		FLAG_STEALTH,
		FLAG_FORGETFUL,
		FLAG_FREEZE_ATTACKER,
		FLAG_FREEZED,
		FLAG_WINDFURY,
		FLAG_POISONOUS,
		FLAG_MAX
	};

	typedef std::array<int, FLAG_MAX> Flags;

public:
	MinionStat() : attack(0), hp(0), max_hp(0), flags(), spell_damage(0)
	{
	}

	bool operator==(MinionStat const& rhs) const
	{
		if (this->attack != rhs.attack) return false;
		if (this->hp != rhs.hp) return false;
		if (this->max_hp != rhs.max_hp) return false;
		if (this->flags != rhs.flags) return false;
		if (this->spell_damage != rhs.spell_damage) return false;
		return true;
	}

	bool operator!=(MinionStat const& rhs) const { return !(*this == rhs); }

	int GetAttack() const { return this->attack; }
	int GetHP() const { return this->hp; }
	int GetMaxHP() const { return this->max_hp; }
	int GetSpellDamage() const { return this->spell_damage; }
	bool IsTaunt() const { return this->flags[MinionStat::FLAG_TAUNT] > 0; }
	bool IsCharge() const { return this->flags[MinionStat::FLAG_CHARGE] > 0; }
	bool IsShield() const { return this->flags[MinionStat::FLAG_SHIELD] > 0; }
	bool IsStealth() const { return this->flags[MinionStat::FLAG_STEALTH] > 0; }
	bool IsForgetful() const { return this->flags[MinionStat::FLAG_FORGETFUL] > 0; }
	bool IsFreezeAttacker() const { return this->flags[MinionStat::FLAG_FREEZE_ATTACKER] > 0; }
	bool IsFreezed() const { return this->flags[MinionStat::FLAG_FREEZED] > 0; }
	bool IsWindFury() const { return this->flags[MinionStat::FLAG_WINDFURY] > 0; }
	bool IsPoisonous() const { return this->flags[MinionStat::FLAG_POISONOUS] > 0; }

	void SetAttack(int attack) { this->attack = attack; }
	void SetHP(int hp) { this->hp = hp; }
	void SetMaxHP(int max_hp) { this->max_hp = max_hp; }
	void SetSpellDamage(int spell_damage) { this->spell_damage = spell_damage; }

	// Add one count to the flag
	void SetTaunt() { this->SetFlag(FLAG_TAUNT); }
	void SetCharge() { this->SetFlag(FLAG_CHARGE); }
	void SetShield() { this->SetFlag(FLAG_SHIELD); }
	void SetStealth() { this->SetFlag(FLAG_STEALTH); }
	void SetForgetful() { this->SetFlag(FLAG_FORGETFUL); }
	void SetFreezeAttacker() { this->SetFlag(FLAG_FREEZE_ATTACKER); }
	void SetFreezed() { this->SetFlag(FLAG_FREEZED); }
	void SetWindFury() { this->SetFlag(FLAG_WINDFURY); }
	void SetPoisonous() { this->SetFlag(FLAG_POISONOUS); }
	void SetFlag(Flag flag)
	{
		++this->flags[flag];
	}

	// Get count of the flag
	int GetForgetfulCount() const { return this->GetFlagCount(FLAG_FORGETFUL); }
	int GetFlagCount(Flag flag) const
	{
		return this->flags[flag];
	}

	// Remove one count from the flag
	void RemoveFlag(Flag flag)
	{
		if (this->flags[flag] > 0) {
			--this->flags[flag];
		}
	}

	// Clear all counts of the flag
	void ClearTaunt() { this->ClearFlag(FLAG_TAUNT); }
	void ClearCharge() { this->ClearFlag(FLAG_CHARGE); }
	void ClearShield() { this->ClearFlag(FLAG_SHIELD); }
	void ClearStealth() { this->ClearFlag(FLAG_STEALTH); }
	void ClearForgetful() { this->ClearFlag(FLAG_FORGETFUL); }
	void ClearFreezeAttacker() { this->ClearFlag(FLAG_FREEZE_ATTACKER); }
	void ClearFreezed() { this->ClearFlag(FLAG_FREEZED); }
	void ClearWindFury() { this->ClearFlag(FLAG_WINDFURY); }
	void ClearPoisonous() { this->ClearFlag(FLAG_POISONOUS); }
	void ClearFlag(Flag flag)
	{
		this->flags[flag] = 0;
	}

private:
	int attack;
	int hp;
	int max_hp;
	Flags flags;
	int spell_damage;
};

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::MinionStat> {
		typedef GameEngine::MinionStat argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.attack);
			GameEngine::hash_combine(result, s.hp);
			GameEngine::hash_combine(result, s.max_hp);
			GameEngine::hash_combine(result, s.spell_damage);
			for (auto const& flag : s.flags) {
				GameEngine::hash_combine(result, flag);
			}

			return result;
		}
	};
}