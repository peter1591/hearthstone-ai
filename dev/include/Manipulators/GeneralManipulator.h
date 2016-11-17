#pragma once

#include "Entity/Card.h"
#include "Manipulators/Helpers/BasicHelper.h"
#include "Manipulators/Helpers/EnchantmentHelper.h"
#include "Manipulators/Helpers/AuraHelper.h"
#include "Manipulators/Helpers/ZonePositionSetter.h"
#include "Manipulators/Helpers/ZoneChanger.h"

class EntitiesManager;

namespace Manipulators
{
	class GeneralManipulator
	{
	public:
		GeneralManipulator(EntitiesManager &mgr, CardRef card_ref, Entity::Card &card) :
			mgr_(mgr), card_ref_(card_ref), card_(card)
		{
		}

		void SetCost(int new_cost) { Helpers::BasicHelper::SetCost(card_, new_cost); }

	public:
		Helpers::EnchantmentHelper GetEnchantmentHelper() { return Helpers::EnchantmentHelper(card_); }
		Helpers::AuraHelper GetAuraHelper() { return Helpers::AuraHelper(mgr_, card_); }
		Helpers::ZonePositionSetter GetZonePositionSetter() { return Helpers::ZonePositionSetter(card_); }

		Helpers::ZoneChangerWithUnknownZoneUnknownType GetZoneChanger()
		{
			// TODO: Can specialize the zone changer to accelerate when moving from a non-play zone to another non-play zone
			// For example: deck --> hand
			return Helpers::ZoneChangerWithUnknownZoneUnknownType(mgr_, card_ref_, card_);
		}

	private:
		EntitiesManager & mgr_;
		CardRef card_ref_;
		Entity::Card & card_;
	};
}