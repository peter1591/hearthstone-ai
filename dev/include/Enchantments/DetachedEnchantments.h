#pragma once

#include <list>
#include <memory>
#include "CloneableContainers/RemovablePtrVector.h"
#include "Enchantment/Base.h"

class DetachedEnchantments
{
public:
	typedef std::list<std::unique_ptr<Enchantment::Base>> ContainerType;

	template <typename T>
	void PushBack(T && item)
	{
		return enchantments_.push_back(std::forward<T>(item));
	}

private:
	ContainerType enchantments_;
};