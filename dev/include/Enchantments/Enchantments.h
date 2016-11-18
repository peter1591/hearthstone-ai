#pragma once

#include <functional>
#include <utility>
#include <memory>
#include "CloneableContainers/RemovableVector.h"

namespace Entity
{
	class Card;
}

class Enchantments
{
public:
	typedef std::function<void(Entity::Card &)> ApplyFunctor;
	typedef CloneableContainers::RemovableVector<ApplyFunctor> ContainerType;

	template <typename T>
	typename ContainerType::Identifier PushBack(T && item)
	{
		return enchantments_.PushBack(item.apply_functor);
	}

	template <typename T>
	void Remove(T&& id)
	{
		return enchantments_.Remove(std::forward<T>(id));
	}

	void ApplyAll(Entity::Card & card)
	{
		enchantments_.IterateAll([&card](ApplyFunctor& functor) -> bool {
			functor(card);
			return true;
		});
	}

private:
	ContainerType enchantments_;
};