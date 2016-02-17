#pragma once

#include <list>
#include "minion.h"

namespace GameEngine {
namespace BoardObjects {

class Minions;

class MinionConstIteratorWithSlotIndex
{
public:
	typedef std::list<Minion> container_type;

public:
	MinionConstIteratorWithSlotIndex(container_type const& container, container_type::const_iterator it_begin, SlotIndex slot_idx_begin)
		: container(container), it(it_begin), slot_idx(slot_idx_begin) { }
	
	bool IsEnd() const;
	void GoToNext();

	Minion const* operator->() { return &(*this->it); }
	Minion const* operator->() const { return &(*this->it); }
	Minion const& operator*() { return *this->it; }
	Minion const& operator*() const { return *this->it; }

	SlotIndex GetSlotIdx() const { return this->slot_idx; }

private:
	container_type const& container;
	container_type::const_iterator it;
	SlotIndex slot_idx;
};

} // BoardObjects
} // GameEngine
