#pragma once

#include <functional>
#include <utility>
#include <memory>
#include "CloneableContainers/RemovableVector.h"

namespace State
{
	namespace Cards
	{
		class Card;
	}
}

class Enchantments
{
public:
	typedef std::function<void(State::Cards::Card &)> ApplyFunctor;
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

	void ApplyAll(State::Cards::Card & card)
	{
		enchantments_.IterateAll([&card](ApplyFunctor& functor) -> bool {
			functor(card);
			return true;
		});
	}

private:
	ContainerType enchantments_;
};