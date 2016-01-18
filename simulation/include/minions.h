#ifndef _MINIONS_H
#define _MINIONS_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include "minion.h"
#include "common.h"

#define MAX_MINIONS 7

class Minions
{
	public:
		Minions();

		// Insert minion to where the index is 'idx'
		// Any existing minions will be shifted right
		void AddMinion(const Minion &minion, int idx);

		// Push the minion to be the rightmost
		void AddMinion(const Minion &minion);

		void DebugPrint() const;

	private:
		std::vector<Minion> minions;
};

inline Minions::Minions()
{
	this->minions.reserve(MAX_MINIONS);
}

inline void Minions::AddMinion(const Minion &minion, int idx)
{
	this->minions.push_back(this->minions.back());
	for (int i=this->minions.size()-1; i>idx; --i) {
		this->minions[i] = this->minions[i-1];
	}
	this->minions[idx] = minion;
}

inline void Minions::AddMinion(const Minion &minion)
{
	this->minions.push_back(minion);
}

inline void Minions::DebugPrint() const
{
	for (const auto &minion : this->minions) {
		if (minion.IsValid()) {
			std::cout << "\t[" << minion.card_id << "] " << minion.hp << " / " << minion.max_hp << std::endl;
		} else {
			std::cout << "\t[EMPTY]" << std::endl;
		}
	}
}

#endif
