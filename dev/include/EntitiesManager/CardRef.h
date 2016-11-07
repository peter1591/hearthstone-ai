#pragma once

#include "Entities/Card.h"
#include "CloneableContainers/Vector.h"

class CardRef
{
public:
	explicit CardRef(typename CloneableContainers::Vector<Card>::Identifier id) : id(id) {}

	typename CloneableContainers::Vector<Card>::Identifier id;
};