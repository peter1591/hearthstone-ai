#pragma once

#include "list-item.h"

namespace GameEngine
{
	template<typename ItemType>
	inline ManagedListItem<ItemType>::ManagedListItem(
		typename ManagedListItem<ItemType>::Container & container,
		typename ManagedListItem<ItemType>::UnderlyingIterator it) :
		container(container), it(it)
	{

	}

	template<typename ItemType>
	inline void ManagedListItem<ItemType>::Remove()
	{
		this->container->Erase(this->it);
	}
} // namespace GameEngine