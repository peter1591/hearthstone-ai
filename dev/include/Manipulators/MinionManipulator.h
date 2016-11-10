#pragma once

#include "Entity/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"
#include "Manipulators/Helpers/EnchantmentHelper.h"
#include "Manipulators/Helpers/AuraHelper.h"
#include "Manipulators/Helpers/ZonePositionSetter.h"

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
		Helpers::EnchantmentHelper GetEnchantmentHelper() { return Helpers::EnchantmentHelper(card_.GetMutableEnchantmentAuxData()); }
		Helpers::AuraHelper GetAuraHelper() { return Helpers::AuraHelper(mgr_, card_.GetMutableAuraAuxData()); }
		Helpers::ZonePositionSetter GetZonePositionSetter() { return Helpers::ZonePositionSetter(card_); }

	private:
		EntitiesManager & mgr_;
		Entity::Card & card_;
	};
}