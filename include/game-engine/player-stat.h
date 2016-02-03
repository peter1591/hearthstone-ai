#ifndef GAME_ENGINE_PLAYER_STAT_H
#define GAME_ENGINE_PLAYER_STAT_H

#include <string>
#include <sstream>
#include <functional>
#include "weapon.h"
#include "hero-power.h"

namespace GameEngine {

class PlayerStat
{
	public:
		class Crystal {
			friend struct std::hash<Crystal>;

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

				bool operator==(const Crystal &rhs) const;
				bool operator!=(const Crystal &rhs) const { return !(*this == rhs); }

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

		int fatigue_damage;

		std::string GetDebugString() const;

		bool operator==(const PlayerStat &rhs) const;
		bool operator!=(const PlayerStat &rhs) const { return !(*this == rhs); }
};

inline std::string PlayerStat::GetDebugString() const
{
	std::ostringstream oss;
	oss << "HP: " << this->hp << " + " << this->armor
		<< ", crystals: "
		<< this->crystal.GetCurrent() << "/" << this->crystal.GetTotal()
		<< ", locked: "
		<< this->crystal.GetLocked() << "/" << this->crystal.GetLockedNextTurn()
		<< ", fatigue: " << this->fatigue_damage;
	return oss.str();
}

inline bool PlayerStat::operator==(const PlayerStat &rhs) const
{
	if (this->hp != rhs.hp) return false;
	if (this->armor != rhs.armor) return false;
	if (this->crystal != rhs.crystal) return false;
	if (this->weapon != rhs.weapon) return false;
	if (this->hero_power != rhs.hero_power) return false;
	if (this->fatigue_damage != rhs.fatigue_damage) return false;

	return true;
}

inline bool PlayerStat::Crystal::operator==(const GameEngine::PlayerStat::Crystal &rhs) const
{
	if (this->current != rhs.current) return false;
	if (this->total != rhs.total) return false;
	if (this->locked != rhs.locked) return false;
	if (this->locked_next_turn != rhs.locked_next_turn) return false;

	return true;
}

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::PlayerStat::Crystal> {
		typedef GameEngine::PlayerStat::Crystal argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.current);
			GameEngine::hash_combine(result, s.total);
			GameEngine::hash_combine(result, s.locked);
			GameEngine::hash_combine(result, s.locked_next_turn);

			return result;
		}
	};

	template <> struct hash<GameEngine::PlayerStat> {
		typedef GameEngine::PlayerStat argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			GameEngine::hash_combine(result, s.hp);
			GameEngine::hash_combine(result, s.armor);
			GameEngine::hash_combine(result, s.crystal);
			GameEngine::hash_combine(result, s.weapon);
			GameEngine::hash_combine(result, s.hero_power);
			GameEngine::hash_combine(result, s.fatigue_damage);

			return result;
		}
	};
}

#endif
