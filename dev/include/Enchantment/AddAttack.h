#pragma once

#include "Enchantment/Base.h"

namespace Enchantment
{
	class AddAttack : public Base
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentTier1;

		void Apply(Entity::Card & card)
		{

		}

		std::unique_ptr<Base> Clone()
		{
			return std::unique_ptr<Base>(new AddAttack(*this));
		}
	};
}