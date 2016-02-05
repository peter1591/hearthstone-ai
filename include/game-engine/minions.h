#ifndef GAME_ENGINE_MINIONS_H
#define GAME_ENGINE_MINIONS_H

#include <functional>
#include <iostream>
#include <vector>
#include <stdexcept>
#include "minion.h"
#include "common.h"
#include "targetor.h"

#define MAX_MINIONS 7

namespace GameEngine {

class Minions
{
	friend std::hash<Minions>;

	public:
		typedef std::vector<Minion> container_type;

	public:
		Minions();

		// Insert minion to where the index is 'idx'
		// Any existing minions will be shifted right
		Minion& AddMinion(const Minion &minion, size_t idx);

		// Push the minion to be the rightmost
		Minion& AddMinion(const Minion &minion);

		int GetMinionCount() const { return (int)this->minions.size(); }

		container_type::const_iterator MinionsBegin() const { return this->minions.cbegin(); }
		container_type::iterator MinionsBegin() { return this->minions.begin(); }
		container_type::const_iterator MinionsEnd() const { return this->minions.cend(); }
		container_type::iterator MinionsEnd() { return this->minions.end(); }
		container_type::iterator MinionsErase(container_type::iterator it) { return this->minions.erase(it); }

		bool IsFull() const { return this->minions.size() == MAX_MINIONS; }

		void DebugPrint() const;

		bool operator==(const Minions &rhs) const;
		bool operator!=(const Minions &rhs) const {
			return !(*this == rhs);
		}

	public: // hooks
		void TurnEnd();

	private:
		std::vector<Minion> minions;
};

inline Minions::Minions()
{
	this->minions.reserve(MAX_MINIONS);
}

inline Minion& Minions::AddMinion(const Minion &minion, size_t idx)
{
	if (idx == this->minions.size()) {
		return this->AddMinion(minion); // push to the rightmost
	}

	this->minions.push_back(this->minions.back());
	for (size_t i=this->minions.size()-1; i>idx; --i) {
		this->minions[i] = this->minions[i-1];
	}
	this->minions[idx] = minion;
	return this->minions[idx];
}

inline Minion& Minions::AddMinion(const Minion &minion)
{
	this->minions.push_back(minion);
	return this->minions.back();
}

inline void Minions::DebugPrint() const
{
	for (const auto &minion : this->minions) {
		if (minion.IsValid()) {
			std::cout << "\t[" << minion.GetCardId() << "] " << minion.GetAttack() << " / " << minion.GetHP() << " (max hp = " << minion.GetMaxHP() << ")";

			if (minion.IsTaunt()) std::cout << " [TAUNT]";
			if (minion.IsCharge()) std::cout << " [CHARGE]";
			if (minion.IsShield()) std::cout << " [SHIELD]";
			if (minion.IsStealth()) std::cout << " [STEALTH]";
			if (minion.IsForgetful()) std::cout << " [FORGETFUL]";

			std::cout << std::endl;
		} else {
			std::cout << "\t[EMPTY]" << std::endl;
		}
	}
}

inline bool Minions::operator==(const Minions &rhs) const
{
	if (this->minions != rhs.minions) return false;

	return true;
}

inline void Minions::TurnEnd()
{
	for (auto &minion : this->minions)
	{
		minion.TurnEnd();
	}
}

} // namespace GameEngine

namespace std {

	template <> struct hash<GameEngine::Minions> {
		typedef GameEngine::Minions argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			result_type result = 0;

			for (auto minion: s.minions) {
				GameEngine::hash_combine(result, minion);
			}

			return result;
		}
	};

}

#endif
