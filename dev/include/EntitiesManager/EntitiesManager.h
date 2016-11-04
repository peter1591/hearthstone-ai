#pragma once

#include <utility>
#include "CloneableContainers/Vector.h"
#include "EntitiesManager/Card.h"
#include "EntitiesManager/CardRef.h"
#include "EntitiesManager/CardManipulator.h"

class EntitiesManager
{
public:
	typedef CloneableContainers::Vector<Card> ContainerType;

	const Card & Get(const CardRef & id) const
	{
		return cards_.Get(id.id);
	}

	template <typename T>
	CardRef PushBack(T&& card)
	{
		return CardRef(cards_.PushBack(std::forward<T>(card)));
	}

	CardManipulator GetManipulator(const CardRef & id)
	{
		return CardManipulator(cards_.Get(id.id));
	}

private:
	ContainerType cards_;
};