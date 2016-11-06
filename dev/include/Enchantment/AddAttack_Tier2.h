#pragma once

#include "Enchantment/Base.h"

namespace Enchantment
{
	class AddAttack_Tier2 : public Base
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentTier2;

		void Apply(Card & card)
		{

		}

		std::unique_ptr<Base> Clone()
		{
			return std::unique_ptr<Base>(new AddAttack_Tier2(*this));
		}
	};
}