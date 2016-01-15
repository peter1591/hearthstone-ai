#ifndef _MINIONS_H
#define _MINIONS_H

#include <iostream>
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
		Minion minions[MAX_MINIONS];
		int minions_len;
};

inline Minions::Minions() : minions_len(0)
{
}

inline void Minions::AddMinion(const Minion &minion, int idx)
{
	if (UNLIKELY(minions_len >= MAX_MINIONS)) {
		throw std::runtime_error("Too many minions");
	}

	if (this->minions[idx].IsValid()) {
		// shift the minions to right
		for (int i=MAX_MINIONS-1; i>idx; ++i) {
			this->minions[i] = this->minions[i-1];
		}
	}
	this->minions[idx] = minion;
	++minions_len;
}

inline void Minions::AddMinion(const Minion &minion)
{
	if (UNLIKELY(minions_len >= MAX_MINIONS)) {
		throw std::runtime_error("Too many minions");
	}

	this->minions[minions_len] = minion;
	++minions_len;
}

inline void Minions::DebugPrint() const
{
	for (int i=0; i<MAX_MINIONS; ++i) {
		const Minion &minion = this->minions[i];
		if (minion.IsValid()) {
			std::cout << "\t[" << minion.card_id << "] " << minion.hp << " / " << minion.max_hp << std::endl;
		} else {
			std::cout << "\t[EMPTY]" << std::endl;
		}
	}
}

#endif
