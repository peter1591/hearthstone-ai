#pragma once

#include "Enchantment/Base.h"

namespace Enchantment
{
	class AddAttack_Aura : public Base
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentAura;

		void Apply(Card & card)
		{

		}

		std::unique_ptr<Base> Clone()
		{
			return std::unique_ptr<Base>(new AddAttack_Aura(*this));
		}
	};
}