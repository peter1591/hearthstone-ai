#pragma once

#include "Entity/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"
#include "Manipulators/Helpers/EnchantmentHelper.h"

namespace Manipulators
{
	class MinionManipulator
	{
	public:
		MinionManipulator(Entity::Card &card) : card_(card), enchantment_helper_(card.GetMutableEnchantmentAuxData())
		{
		}

		void ChangeZone(Entity::CardZone new_zone)
		{
			card_.SetZone(new_zone);
		}

		void SetCost(int new_cost)
		{
			Helpers::BasicHelper::SetCost(card_, new_cost);
		}

	public:
		Helpers::EnchantmentHelper & GetEnchantmentHelper() { return enchantment_helper_; }

	private:
		Entity::Card & card_;
		Helpers::EnchantmentHelper enchantment_helper_;
	};
}