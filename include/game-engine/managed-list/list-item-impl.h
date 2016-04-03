#pragma once

#include "list-item.h"

namespace GameEngine
{
	template<typename ItemType>
	inline ManagedListItem<ItemType>::ManagedListItem(typename ManagedListItem<ItemType>::UnderlyingIterator it)
		: it(it)
	{
	}

	template<typename ItemType>
	inline void ManagedListItem<ItemType>::Remove(Container & container)
	{
		container->Erase(this->it);
	}
} // namespace GameEngine