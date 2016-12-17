#pragma once

#include "State/Cards/Card.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulators/Helpers/AuraHelper.h"
#include "FlowControl/Manipulators/Helpers/DeathrattlesHelper.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper.h"
#include "FlowControl/Manipulators/Helpers/ZonePositionSetter.h"
#include "FlowControl/Manipulators/Helpers/ZoneChanger.h"
#include "FlowControl/Manipulators/Helpers/DamageHelper.h"
#include "State/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace detail
		{
			class DamageSetter
			{
				friend class Helpers::DamageHelper;
			public:
				DamageSetter(state::Cards::Card & card) : card_(card) {}

			private:
				void TakeDamage(int v) { card_.GetDamageSetter().Set(card_.GetDamage() + v); }

			private:
				state::Cards::Card & card_;
			};
		}

		class CardManipulator
		{
		public:
			CardManipulator(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card) :
				state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
			{
			}

		public:
			void Cost(int new_cost) { card_.SetCost(new_cost); }
			void Attack(int new_attack) { card_.SetAttack(new_attack); }
			void MaxHP(int new_max_hp) { card_.SetMaxHP(new_max_hp); }

			void Taunt(bool v) { card_.SetTaunt(v); }
			void Shield(bool v) { card_.SetShield(v); }

			Helpers::EnchantmentHelper Enchant() { return Helpers::EnchantmentHelper(state_, flow_context_, card_ref_, card_); }
			Helpers::AuraHelper Aura() { return Helpers::AuraHelper(state_, flow_context_, card_ref_, card_); }
			Helpers::DeathrattlesHelper Deathrattles() { return Helpers::DeathrattlesHelper(state_, flow_context_, card_ref_, card_); }

			Helpers::ZonePositionSetter ZonePosition() { return Helpers::ZonePositionSetter(card_); }
			Helpers::ZoneChangerWithUnknownZoneUnknownType Zone()
			{
				// TODO: Can specialize the zone changer to accelerate when moving from a non-play zone to another non-play zone
				// For example: deck --> hand
				return Helpers::ZoneChangerWithUnknownZoneUnknownType(state_, flow_context_, card_ref_, card_);
			}

			Helpers::DamageHelper Damage() { return Helpers::DamageHelper(state_, flow_context_, card_ref_, card_); }

		protected:
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			state::Cards::Card & card_;
		};
	}
}