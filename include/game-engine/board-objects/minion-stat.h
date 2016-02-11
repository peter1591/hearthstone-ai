#pragma once

#include <array>
#include "game-engine/common.h"

namespace GameEngine {
namespace BoardObjects {

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
		FLAG_MAX
	};

	typedef std::array<int, FLAG_MAX> Flags;

public:
	MinionStat() : attack(0), hp(0), max_hp(0), flags()
	{
	}

	bool operator==(MinionStat const& rhs) const
	{
		if (this->attack != rhs.attack) return false;
		if (this->hp != rhs.hp) return false;
		if (this->max_hp != rhs.max_hp) return false;
		if (this->flags != rhs.flags) return false;
		return true;
	}

	bool operator!=(MinionStat const& rhs) const { return !(*this == rhs); }

	int GetAttack() const { return this->attack; }
	int GetHP() const { return this->hp; }
	int GetMaxHP() const { return this->max_hp; }
	bool IsTaunt() const { return this->flags[MinionStat::FLAG_TAUNT] > 0; }
	bool IsCharge() const { return this->flags[MinionStat::FLAG_CHARGE] > 0; }
	bool IsShield() const { return this->flags[MinionStat::FLAG_SHIELD] > 0; }
	bool IsStealth() const { return this->flags[MinionStat::FLAG_STEALTH] > 0; }
	bool IsForgetful() const { return this->flags[MinionStat::FLAG_FORGETFUL] > 0; }
	bool IsFreezeAttacker() const { return this->flags[MinionStat::FLAG_FREEZE_ATTACKER] > 0; }
	bool IsFreezed() const { return this->flags[MinionStat::FLAG_FREEZED] > 0; }
	bool IsWindFury() const { return this->flags[MinionStat::FLAG_WINDFURY] > 0; }

	void SetAttack(int attack) { this->attack = attack; }
	void SetHP(int hp) { this->hp = hp; }
	void SetMaxHP(int max_hp) { this->max_hp = max_hp; }
	void SetTaunt(bool val) { this->SetFlag(FLAG_TAUNT, val); }
	void SetCharge(bool val) { this->SetFlag(FLAG_CHARGE, val); }
	void SetShield(bool val) { this->SetFlag(FLAG_SHIELD, val); }
	void SetStealth(bool val) { this->SetFlag(FLAG_STEALTH, val); }
	void SetForgetful(bool val) { this->SetFlag(FLAG_FORGETFUL, val); }
	void SetFreezeAttacker(bool val) { this->SetFlag(FLAG_FREEZE_ATTACKER, val); }
	void SetFreezed(bool val) { this->SetFlag(FLAG_FREEZED, val); }
	void SetWindFury(bool val) { this->SetFlag(FLAG_WINDFURY, val); }

	void SetFlag(Flag flag, bool set)
	{
		if (set) {
			++this->flags[flag];
		}
		else {
			--this->flags[flag];
		}
	}

private:
	int attack;
	int hp;
	int max_hp;
	Flags flags;
};

}
}

namespace std {

	template <> struct hash<GameEngine::BoardObjects::MinionStat> {
		typedef GameEngine::BoardObjects::MinionStat argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.attack);
			GameEngine::hash_combine(result, s.hp);
			GameEngine::hash_combine(result, s.max_hp);
			for (auto const& flag : s.flags) {
				GameEngine::hash_combine(result, flag);
			}

			return result;
		}
	};
}