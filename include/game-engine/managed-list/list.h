#pragma once

#include <list>
#include "game-engine/managed-list/list-item.h"

namespace GameEngine
{
	// Note:
	// The iterator is missing by design.
	// This container makes sure all the iterators instances (i.e., the ManagedItem) are valid
	//      until the container vanishes.
	// If a iterator class is introduced, we cannot guarantee the iterator's lifetime

	template <typename ItemType>
	class ManagedList
	{
	public:
		using ManagedItem = ManagedListItem<ItemType>;
		using UnderlyingContainer = typename ManagedItem::UnderlyingContainer;

		ManagedList() {}
		ManagedList(ManagedList<ItemType> && rhs)
		{
			this->items.splice(this->items.begin(), std::move(rhs.items)); // maintain the iterator (i.e., ManagedItem) validity
		}

		bool operator==(ManagedList const& rhs) const { return this->items == rhs.items; }
		bool operator!=(ManagedList const& rhs) const { return this->items != rhs.items; }

		bool Empty() const { return this->items.empty(); }

		ManagedItem PushFront(ItemType const& item) { return ManagedItem(this->items.insert(this->items.begin(), item)); }
		ManagedItem PushFront(ItemType && item) { return ManagedItem(this->items.insert(this->items.begin(), std::move(item))); }

		ManagedItem PushBack(ItemType const& item) { return ManagedItem(this->items.insert(this->items.end(), item)); }
		ManagedItem PushBack(ItemType && item) { return ManagedItem(this->items.insert(this->items.end(), std::move(item))); }

		// TODO: remove_if
		// TODO: for_each

	public: // only used by ManagedListIterator
		void Erase(typename UnderlyingContainer::iterator it) { this->items.erase(it); }

	private:
		UnderlyingContainer items;
	};

} // namespace GameEngine

#include "game-engine/managed-list/list-item-impl.h"