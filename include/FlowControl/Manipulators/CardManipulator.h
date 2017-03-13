#pragma once

#include "state/Cards/Card.h"
#include "state/ZoneChanger.h"
#include "FlowControl/Manipulators/Helpers/AuraHelper.h"
#include "FlowControl/Manipulators/Helpers/DeathrattlesHelper.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper.h"
#include "FlowControl/Manipulators/detail/DamageSetter.h"
#include "FlowControl/FlowContext.h"

namespace state {
	class State;
}

namespace FlowControl
{
	namespace Manipulators
	{
		class CardManipulator
		{
		public:
			CardManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card &card) :
				state_(state), flow_context_(flow_context), card_ref_(card_ref), card_(card)
			{
			}

			void Cost(int new_cost) { card_.SetCost(new_cost); }
			void Attack(int new_attack) { card_.SetAttack(new_attack); }
			void MaxHP(int new_max_hp) { card_.SetMaxHP(new_max_hp); }

			void SpellDamage(int v) { card_.SetSpellDamage(v); }

			detail::DamageSetter Internal_SetDamage() { return detail::DamageSetter(card_); }

		public:
			Helpers::EnchantmentHelper Enchant() { return Helpers::EnchantmentHelper(state_, flow_context_, card_ref_, card_); }
			Helpers::AuraHelper Aura() { return Helpers::AuraHelper(state_, flow_context_, card_ref_, card_); }
			Helpers::DeathrattlesHelper Deathrattles() { return Helpers::DeathrattlesHelper(state_, flow_context_, card_ref_, card_); }

		protected:
			state::State & state_;
			FlowControl::FlowContext & flow_context_;
			state::CardRef card_ref_;
			state::Cards::Card & card_;
		};
	}
}