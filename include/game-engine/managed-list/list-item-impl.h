#pragma once

#include "list-item.h"

namespace GameEngine
{
	template<typename ItemType>
	inline ManagedListItem<ItemType>::ManagedListItem(Container & container, typename ManagedListItem<ItemType>::UnderlyingIterator it)
		: container(container), it(it)
	{
	}

	template<typename ItemType>
	inline ManagedListItem<ItemType>::ManagedListItem(ManagedListItem<ItemType> const & rhs)
		: container(rhs.container), it(rhs.it)
	{
	}

	template<typename ItemType>
	inline ManagedListItem<ItemType>::ManagedListItem(ManagedListItem<ItemType>&& rhs)
		: container(rhs.container), it(std::move(rhs.it))
	{
	}

	template<typename ItemType>
	inline void ManagedListItem<ItemType>::Remove()
	{
		this->container.Erase(this->it);
	}
} // namespace GameEngine