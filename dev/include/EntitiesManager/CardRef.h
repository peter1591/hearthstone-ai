#pragma once

#include <type_traits>
#include "Entity/Card.h"
#include "CloneableContainers/Vector.h"

class CardRef
{
public:
	explicit CardRef(typename CloneableContainers::Vector<Entity::Card>::Identifier id) : id(id) {}

	template <typename T>
	bool operator==(T&& rhs) const
	{
		static_assert(std::is_same<std::decay_t<T>, CardRef>::value, "Wrong type");
		return this->id == rhs.id;
	}

	template <typename T> bool operator!=(T&& rhs) const
	{
		return !(*this == rhs);
	}

	typename CloneableContainers::Vector<Entity::Card>::Identifier id;
};

namespace std
{
	template <>
	struct hash<CardRef>
	{
		std::size_t operator()(const CardRef& key) const
		{
			return CloneableContainers::Vector<Entity::Card>::IdentifierHasher()(key.id);
		}
	};
}