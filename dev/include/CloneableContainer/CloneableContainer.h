#pragma once

#include <vector>

// This is a cloneable container, since the identifier is defined to be the index
// On the contrary, if we use raw pointer as the identifier, those identifiers should be
//    changed as well after cloned.
// BTW, this comes with a price: items cannot be erased/removed

template <class ItemType>
class CloneableContainer
{
public:
	typedef int Identifier;

	CloneableContainer() {}
	CloneableContainer(size_t default_capacity) {
		items_.reserve(default_capacity);
	}

	template <typename Args...>
	Identifier Create(const Args&... args) {
		int idx = items_.size();
		items_.push_back(ItemType(args...));
		return idx;
	}

	const ItemType & Get(Identifier identifier) const {
		return items_[identifier];
	}

	ItemType & Get(Identifier identifier) {
		return items_[identifier];
	}

private:
	std::vector<ItemType> items_;
};