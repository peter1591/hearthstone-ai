#pragma once

#include "Entity/Card.h"
#include "CloneableContainers/Vector.h"

class CardRef
{
public:
	explicit CardRef(typename CloneableContainers::Vector<Entity::Card>::Identifier id) : id(id) {}

	typename CloneableContainers::Vector<Entity::Card>::Identifier id;
};