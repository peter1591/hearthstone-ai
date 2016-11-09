#pragma once

#include <utility>
#include <memory>
#include "CloneableContainers/RemovablePtrVector.h"
#include "Enchantment/Base.h"

class AttachedEnchantments
{
public:
	typedef CloneableContainers::RemovablePtrVector<Enchantment::Base*> ContainerType;

	template <typename T>
	typename ContainerType::Identifier PushBack(T && item)
	{
		return enchantments_.PushBack(std::forward<T>(item));
	}

	template <typename T>
	Enchantment::Base* Get(T&& id) const
	{
		return enchantments_.Get(std::forward<T>(id));
	}

	template <typename T>
	void Remove(T&& id) const
	{
		return enchantments_.Remove(std::forward<T>(id));
	}

private:
	ContainerType enchantments_;
};