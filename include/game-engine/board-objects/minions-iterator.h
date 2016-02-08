#pragma once

#include <list>
#include "minion.h"

namespace GameEngine {
namespace BoardObjects {

class Minions;

class MinionsIteratorWithIndex
{
public:
	typedef std::list<Minion> container_type;

public:
	MinionsIteratorWithIndex(SlotIndex slot_idx_begin, container_type::iterator it_begin, Minions &container)
		: slot_idx(slot_idx_begin), it(it_begin), container(container) { }

	bool IsEnd() const;

	void GoToNext();
	void EraseAndGoToNext();
	Minion& InsertBefore(Minion && minion);

	container_type::iterator::pointer operator->() { return this->it.operator->(); }

	Minions & GetOwner() const { return this->container; }
	SlotIndex GetSlotIdx() const { return this->slot_idx; }

private:
	SlotIndex slot_idx;
	container_type::iterator it;
	Minions & container;
};

class MinionsConstIteratorWithIndex
{
public:
	typedef std::list<Minion> container_type;

public:
	MinionsConstIteratorWithIndex(SlotIndex slot_idx_begin, container_type::const_iterator it_begin, Minions const& container)
		: slot_idx(slot_idx_begin), it(it_begin), container(container) { }

	bool IsEnd() const;
	void GoToNext();

	container_type::const_iterator::pointer operator->() { return this->it.operator->(); }

	Minions const& GetOwner() const { return this->container; }
	SlotIndex GetSlotIdx() const { return this->slot_idx; }

private:
	SlotIndex slot_idx;
	container_type::const_iterator it;
	Minions const& container;
};

} // BoardObjects
} // GameEngine