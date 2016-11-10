#pragma once

#include "Entity/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"
#include "Manipulators/Helpers/EnchantmentHelper.h"
#include "Manipulators/Helpers/AuraHelper.h"

class EntitiesManager;

namespace Manipulators
{
	class MinionManipulator
	{
	public:
		MinionManipulator(EntitiesManager &mgr, Entity::Card &card);

		void ChangeZone(Entity::CardZone new_zone) { card_.SetZone(new_zone); }
		void SetCost(int new_cost) { Helpers::BasicHelper::SetCost(card_, new_cost); }

	public:
		Helpers::EnchantmentHelper & GetEnchantmentHelper() { return enchantment_helper_; }
		Helpers::AuraHelper & GetAuraHelper() { return aura_helper_; }

	private:
		EntitiesManager & mgr_;
		Entity::Card & card_;
		Helpers::EnchantmentHelper enchantment_helper_;
		Helpers::AuraHelper aura_helper_;
	};
}