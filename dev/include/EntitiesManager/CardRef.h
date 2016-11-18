#pragma once

#include <type_traits>
#include "CloneableContainers/Vector.h"

namespace Entity
{
	class Card;
}

class CardRef
{
public:
	typedef typename CloneableContainers::Vector<Entity::Card>::Identifier IdentifierType;

	CardRef() : id(IdentifierType::GetInvalidIdentifier()) {}
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

	bool IsValid() const { return id.IsValid(); }
	void Invalidate() { id = IdentifierType::GetInvalidIdentifier(); }

	IdentifierType id;
};

namespace std
{
	template <>
	struct hash<CardRef>
	{
		std::size_t operator()(const CardRef& key) const
		{
			return CloneableContainers::VectorIdentifierHasher()(key.id);
		}
	};
}