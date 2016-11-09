#pragma once

#include <memory>
#include "CloneableContainers/RemovablePtrVector.h"
#include "Enchantment/Base.h"

class AttachedEnchantments
{
public:
	typedef CloneableContainers::RemovablePtrVector<Enchantment::Base*> ContainerType;

	Enchantment::Base* Get(typename ContainerType::Identifier id) const
	{
		return enchantments_.Get(id);
	}

	template <typename T>
	typename ContainerType::Identifier PushBack(T && item)
	{
		return enchantments_.PushBack(std::forward<T>(item));
	}

private:
	ContainerType enchantments_;
};