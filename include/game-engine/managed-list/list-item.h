#pragma once

#include <list>

namespace GameEngine
{
	template<typename ItemType> class ManagedList;

	template <typename ItemType>
	class ManagedListItem
	{
	public:
		typedef typename ManagedList<ItemType> Container;
		typedef std::list<ItemType> UnderlyingContainer;
		typedef typename UnderlyingContainer::iterator UnderlyingIterator;

	public:
		ManagedListItem(UnderlyingIterator it);

		void Remove(Container & container);

	private:
		UnderlyingIterator it;
	};
} // namespace GameEngine