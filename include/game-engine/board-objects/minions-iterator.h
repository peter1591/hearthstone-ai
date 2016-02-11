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
	MinionConstIteratorWithSlotIndex(SlotIndex slot_idx_begin, container_type::const_iterator it_begin, Minions const& container)
		: slot_idx(slot_idx_begin), it(it_begin), container(container) { }

	bool IsEnd() const;
	void GoToNext();

	Minion const* operator->() { return &(*this->it); }
	Minion const* operator->() const { return &(*this->it); }
	Minion const& operator*() { return *this->it; }
	Minion const& operator*() const { return *this->it; }

	Minions const& GetOwner() const { return this->container; }
	SlotIndex GetSlotIdx() const { return this->slot_idx; }

private:
	SlotIndex slot_idx;
	container_type::const_iterator it;
	Minions const& container;
};

} // BoardObjects
} // GameEngine
