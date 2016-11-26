#pragma once

#include "State/Cards/Card.h"
#include "State/Manipulators/Helpers/BasicHelper.h"
#include "State/Manipulators/Helpers/EnchantmentHelper.h"
#include "State/Manipulators/Helpers/AuraHelper.h"
#include "State/Manipulators/Helpers/ZonePositionSetter.h"
#include "State/Manipulators/Helpers/ZoneChanger.h"
#include "State/State.h"

namespace state
{
	namespace Manipulators
	{
		class WeaponManipulator
		{
		public:
			WeaponManipulator(State & state, CardRef card_ref, Cards::Card &card) :
				state_(state), card_ref_(card_ref), card_(card)
			{
			}

		public:
			Helpers::EnchantmentHelper Enchant() { return Helpers::EnchantmentHelper(card_); }
			Helpers::AuraHelper Aura() { return Helpers::AuraHelper(state_, card_); }
			Helpers::ZonePositionSetter ZonePosition() { return Helpers::ZonePositionSetter(card_); }
			Helpers::ZoneChangerWithUnknownZone<kCardTypeMinion> Zone() {
				return Helpers::ZoneChangerWithUnknownZone<kCardTypeMinion>(state_, card_ref_, card_);
			}

			void ReduceDurability(int v) {
				card_.SetDamage(card_.GetDamage() + v);
			}

		private:
			State & state_;
			CardRef card_ref_;
			Cards::Card & card_;
		};
	}
}