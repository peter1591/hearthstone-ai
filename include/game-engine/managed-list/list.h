#pragma once

#include <list>
#include "game-engine/managed-list/list-item.h"

namespace GameEngine
{
	template <typename ItemType>
	class ManagedList
	{
	public:
		using ManagedItem = ManagedListItem<ItemType>;
		using UnderlyingContainer = typename ManagedItem::UnderlyingContainer;

		ManagedList() {}
		ManagedList(ManagedList<ItemType> && rhs)
		{
			this->items.splice(this->items.begin(), std::move(rhs)); // maintain the iterator validity
		}

		ManagedItem PushFront(ItemType && item) { return ManagedItem(*this, this->items.push_front(item)); }
		ManagedItem PushBack(ItemType && item) { return ManagedItem(*this, this->items.push_back(item)); }

	public: // only used by ManagedListIterator
		void Erase(typename UnderlyingContainer::iterator it) { this->items.erase(it); }

	private:
		UnderlyingContainer items;
	};

} // namespace GameEngine

#include "game-engine/managed-list/list-item-impl.h"