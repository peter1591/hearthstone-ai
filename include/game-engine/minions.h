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

		enum Side {
			PLAYER_SIDE,
			OPPONENT_SIDE
		};

	public:
		Minions() = delete;
		Minions(Side side);

		// Insert minion to where the index is 'idx'
		// Any existing minions will be shifted right
		void AddMinion(const Minion &minion, size_t idx);

		// Push the minion to be the rightmost
		void AddMinion(const Minion &minion);

		const container_type &GetMinions() const { return this->minions; }
		container_type &GetMinions() { return this->minions; }

		TargetorBitmap GetTargetsToAttack() const;
		TargetorBitmap GetTargetsToBeAttacked(bool &has_taunt) const;

		bool IsFull() const { return this->minions.size() == MAX_MINIONS; }

		void DebugPrint() const;

		bool operator==(const Minions &rhs) const;
		bool operator!=(const Minions &rhs) const {
			return !(*this == rhs);
		}

	public: // hooks
		void TurnEnd();

	private:
		int GetTargetorByMinionIndex(int idx) const;

	private:
		std::vector<Minion> minions;
		Side side;
};

inline Minions::Minions(Side side)
{
	this->minions.reserve(MAX_MINIONS);
	this->side = side;
}

inline void Minions::AddMinion(const Minion &minion, size_t idx)
{
	if (idx == this->minions.size()) {
		// push to the rightmost
		this->minions.push_back(minion);
		return;
	}

	this->minions.push_back(this->minions.back());
	for (size_t i=this->minions.size()-1; i>idx; --i) {
		this->minions[i] = this->minions[i-1];
	}
	this->minions[idx] = minion;
}

inline void Minions::AddMinion(const Minion &minion)
{
	this->minions.push_back(minion);
}

inline TargetorBitmap Minions::GetTargetsToAttack() const
{
	TargetorBitmap targetors;
	for (int i = 0; i < this->minions.size(); ++i)
	{
		auto const& minion = this->minions[i];

		if (!minion.Attackable()) continue;

		targetors.SetOneTarget(this->GetTargetorByMinionIndex(i));
	}
	return targetors;
}

inline TargetorBitmap Minions::GetTargetsToBeAttacked(bool & has_taunt) const
{
	TargetorBitmap targetors;
	
	// 1. Find taunt minions
	for (int i = 0; i < this->minions.size(); ++i)
	{
		auto const& minion = this->minions[i];

		if (!minion.IsTaunt()) continue;

		targetors.SetOneTarget(this->GetTargetorByMinionIndex(i));
	}

	if (!targetors.None())
	{
		has_taunt = true;
		return targetors; // has taunt minions
	}
	has_taunt = false;

	// 2. No taunt minions --> all minions are attackable
	for (int i = 0; i < this->minions.size(); ++i)
	{
		targetors.SetOneTarget(this->GetTargetorByMinionIndex(i));
	}
	return targetors;
}

inline void Minions::DebugPrint() const
{
	for (const auto &minion : this->minions) {
		if (minion.IsValid()) {
			std::cout << "\t[" << minion.GetCardId() << "] " << minion.GetAttack() << " / " << minion.GetHP() << " (max hp = " << minion.GetMaxHP() << ")";

			if (minion.IsTaunt()) std::cout << " [TAUNT]";
			if (minion.IsCharge()) std::cout << " [CHARGE]";

			std::cout << std::endl;
		} else {
			std::cout << "\t[EMPTY]" << std::endl;
		}
	}
}

inline bool Minions::operator==(const Minions &rhs) const
{
#ifdef DEBUG
	if (this->side != rhs.side) throw std::runtime_error("logic error");
#endif

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

inline int Minions::GetTargetorByMinionIndex(int idx) const
{
	if (this->side == PLAYER_SIDE) return Targetor::GetPlayerMinionIndex(idx);
	else return Targetor::GetOpponentMinionIndex(idx);
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
