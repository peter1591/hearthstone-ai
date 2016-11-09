#pragma once

#include <memory>
#include "Entity/Card.h"
#include "Enchantments/Tiers.h"

namespace Enchantment
{
	class Base
	{
	public:
		virtual ~Base() {}

		virtual void Apply(Card & card) = 0;
		virtual std::unique_ptr<Base> Clone() = 0;
	};
}