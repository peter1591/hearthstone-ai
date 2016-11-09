#pragma once

#include <memory>
#include "Enchantments/Tiers.h"

namespace Entity
{
	class Card;
}

namespace Enchantment
{
	class Base
	{
	public:
		virtual ~Base() {}

		virtual void Apply(Entity::Card & card) = 0;
		virtual std::unique_ptr<Base> Clone() = 0;
	};
}