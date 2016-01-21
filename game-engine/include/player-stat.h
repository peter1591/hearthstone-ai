#ifndef PLAYER_STAT_H
#define PLAYER_STAT_H

#include <string>
#include <sstream>
#include "weapon.h"
#include "hero-power.h"

namespace GameEngine {

class PlayerStat
{
	public:
		class Crystal {
			public:
				void Set(int current, int total, int locked, int locked_next_turn) {
					this->current = current;
					this->total = total;
					this->locked = locked;
					this->locked_next_turn = locked_next_turn;
				}

				int GetCurrent() const { return this->current; }
				int GetTotal() const { return this->total; }
				int GetLocked() const { return this->locked; }
				int GetLockedNextTurn() const { return this->locked_next_turn; }

				void CostCrystals(int cost) { this->current -= cost; }
				void LockCrystals(int locked) { this->locked_next_turn -= locked; }
				void TurnStart() {
					this->total++;
					this->locked = this->locked_next_turn;
					this->current = this->total - this->locked;
					this->locked_next_turn = 0;
				}

			private:
				int current;
				int total;
				int locked;
				int locked_next_turn;
		};

	public:
		int hp;
		int armor;

		Crystal crystal;

		Weapon weapon;
		HeroPower hero_power;

		std::string GetDebugString() const;
};

inline std::string PlayerStat::GetDebugString() const
{
	std::ostringstream oss;
	oss << "HP: " << this->hp << " + " << this->armor
		<< ", crystals: "
		<< this->crystal.GetCurrent() << "/" << this->crystal.GetTotal()
		<< ", locked: "
		<< this->crystal.GetLocked() << "/" << this->crystal.GetLockedNextTurn();
	return oss.str();
}

} // namespace GameEngine

#endif
