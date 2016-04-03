#pragma once

#include <list>
#include <functional>

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
		ManagedList(ManagedList<ItemType> const& rhs) = delete;
		ManagedList(ManagedList<ItemType> && rhs) = delete;

		bool operator==(ManagedList const& rhs) const { return this->items == rhs.items; }
		bool operator!=(ManagedList const& rhs) const { return this->items != rhs.items; }

		bool Empty() const { return this->items.empty(); }

		ManagedItem PushFront(ItemType const& item) { return ManagedItem(*this, this->items.insert(this->items.begin(), item)); }
		ManagedItem PushFront(ItemType && item) { return ManagedItem(*this, this->items.insert(this->items.begin(), std::move(item))); }

		ManagedItem PushBack(ItemType const& item) { return ManagedItem(*this, this->items.insert(this->items.end(), item)); }
		ManagedItem PushBack(ItemType && item) { return ManagedItem(*this, this->items.insert(this->items.end(), std::move(item))); }

		void ForEach(std::function<void(ItemType &)> func) {
			for (auto & item : this->items) func(item);
		}
		void ForEach(std::function<void(ItemType const&)> func) const {
			for (auto const& item : this->items) func(item);
		}

		// Note: Invalidate all ManagedItem which are removed
		void RemoveIf(std::function<bool(ManagedItem)> func) {
			for (auto it = this->items.begin(); it != this->items.end();) {
				if (func(ManagedItem(*this, it))) {
					it = this->items.erase(it);
				}
				else {
					++it;
				}
			}
		}

	public: // only used by ManagedListIterator
		void Erase(typename UnderlyingContainer::iterator it) { this->items.erase(it); }

	private:
		UnderlyingContainer items;
	};

} // namespace GameEngine

#include "game-engine/managed-list/list-item-impl.h"