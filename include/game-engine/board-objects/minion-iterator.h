#pragma once

#include <list>
#include "minion-manipulator.h"

namespace GameEngine {
namespace BoardObjects {

class Minions;

class MinionIterator
{
	friend class Minions;

public:
	typedef std::list<Minion> container_type;

public:
	MinionIterator(Board & board, Minions & minions, container_type::iterator it)
		: board(board), minions(minions), it(it) {}

	Board & GetBoard() const { return this->board; }
	Minions & GetMinions() const { return this->minions; }
	container_type::iterator GetIterator() const { return it; }

	void GoToNext();
	bool IsEnd() const;

	Minion const* operator->() { return &(*this->it); }
	Minion const* operator->() const { return &(*this->it); }
	Minion const& operator*() { return *this->it; }
	Minion const& operator*() const { return *this->it; }

	MinionManipulator ConvertToManipulator() { 
		if (this->IsEnd()) throw std::runtime_error("minion vanished");
		return MinionManipulator(this->board, this->minions, *it); 
	}

private:
	Board & board;
	Minions & minions;
	container_type::iterator it;
};

class MinionConstIteratorWithSlotIndex
{
public:
	typedef MinionIterator::container_type container_type;

public:
	MinionConstIteratorWithSlotIndex(Minions const& minions, container_type::const_iterator it_begin, SlotIndex slot_idx_begin)
		: minions(minions), it(it_begin), slot_idx(slot_idx_begin) { }

	void GoToNext();
	bool IsEnd() const;

	Minion const* operator->() { return &(*this->it); }
	Minion const* operator->() const { return &(*this->it); }
	Minion const& operator*() { return *this->it; }
	Minion const& operator*() const { return *this->it; }

	SlotIndex GetSlotIdx() const { return this->slot_idx; }

private:
	Minions const& minions;
	container_type::const_iterator it;
	SlotIndex slot_idx;
};

} // BoardObjects
} // GameEngine
