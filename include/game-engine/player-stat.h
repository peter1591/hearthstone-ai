#ifndef GAME_ENGINE_PLAYER_STAT_H
#define GAME_ENGINE_PLAYER_STAT_H

#include <string>
#include <sstream>
#include <functional>

namespace GameEngine {

	class PlayerStat
	{
	public:
		class Crystal {
			friend struct std::hash<Crystal>;

		public:
			void Set(int current_, int total_, int locked_, int locked_next_turn_) {
				this->current = current_;
				this->total = total_;
				this->locked = locked_;
				this->locked_next_turn = locked_next_turn_;
			}

			int GetCurrent() const { return this->current; }
			int GetTotal() const { return this->total; }
			int GetLocked() const { return this->locked; }
			int GetLockedNextTurn() const { return this->locked_next_turn; }

			void IncreaseCrystalThisTurn(int amount) { this->current += amount; }
			void GainEmptyCrystals(int amount) { this->total += amount; }

			void CostCrystals(int cost) { this->current -= cost; }
			void LockCrystals(int locked_) { this->locked_next_turn -= locked_; }
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
		PlayerStat() : fatigue_damage(0), spell_damage(0) {}

	public:
		Crystal crystal;

		int fatigue_damage;

		int spell_damage;

		std::string GetDebugString() const;

		bool operator==(const PlayerStat &rhs) const;
		bool operator!=(const PlayerStat &rhs) const { return !(*this == rhs); }
	};

	inline std::string PlayerStat::GetDebugString() const
	{
		std::ostringstream oss;
		oss << "crystals: "
			<< this->crystal.GetCurrent() << "/" << this->crystal.GetTotal()
			<< ", locked: "
			<< this->crystal.GetLocked() << "/" << this->crystal.GetLockedNextTurn()
			<< ", next fatigue: " << this->fatigue_damage
			<< ", spell: " << this->spell_damage;
		return oss.str();
	}

	inline bool PlayerStat::operator==(const PlayerStat &rhs) const
	{
		if (this->crystal != rhs.crystal) return false;
		if (this->fatigue_damage != rhs.fatigue_damage) return false;
		if (this->spell_damage!= rhs.spell_damage) return false;

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

			GameEngine::hash_combine(result, s.crystal);
			GameEngine::hash_combine(result, s.fatigue_damage);
			GameEngine::hash_combine(result, s.spell_damage);

			return result;
		}
	};
}

#endif
