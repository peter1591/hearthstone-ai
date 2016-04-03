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
		ManagedListItem(Container & container, UnderlyingIterator it);
		ManagedListItem(ManagedListItem<ItemType> const& rhs);
		ManagedListItem(ManagedListItem<ItemType> && rhs);

	public:
		typename UnderlyingIterator::reference operator*() const { return it.operator*(); }
		typename UnderlyingIterator::pointer operator->() const { return it.operator->(); }

		bool operator==(ManagedListItem<ItemType> const& rhs)
		{
			if (&this->container != &rhs.container) return false;
			return this->it == rhs.it;
		}
		bool operator!=(ManagedListItem<ItemType> const& rhs) { return !(*this == rhs); }

		void Remove();

	private:
		Container & container;
		UnderlyingIterator it;
	};
} // namespace GameEngine