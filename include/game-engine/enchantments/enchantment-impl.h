#pragma once

#include "enchantment.h"

namespace GameEngine
{
	template <typename Target>
	Enchantment<Target>::Enchantment()
	{

	}

	template<typename Target>
	inline Enchantment<Target>::~Enchantment()
	{
	}

	template<typename Target>
	inline bool Enchantment<Target>::operator==(Enchantment<Target> const & rhs) const
	{
		return this->EqualsTo(rhs);
	}

	template<typename Target>
	inline bool Enchantment<Target>::operator!=(Enchantment<Target> const & rhs) const
	{
		return !(*this == rhs);
	}
}