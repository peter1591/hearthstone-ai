#pragma once

#include <list>
#include <memory>
#include "Enchantment/Base.h"
#include "Utils/CopyByCloneWrapper.h"

class DetachedEnchantments
{
private:
	typedef Enchantment::Base ItemType;
	typedef std::unique_ptr<ItemType> ManagedItemType;
	typedef Utils::CopyByPtrCloneWrapper<ManagedItemType> CopyableItemType;

public:
	typedef std::list<CopyableItemType> ContainerType;

	template <typename T>
	void PushBack(T && item)
	{
		return enchantments_.push_back(CopyableItemType(std::forward<T>(item)));
	}

private:
	ContainerType enchantments_;
};