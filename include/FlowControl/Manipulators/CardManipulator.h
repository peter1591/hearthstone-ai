#pragma once

#include "state/Cards/Card.h"
#include "state/ZoneChanger.h"
#include "FlowControl/Manipulators/BoardManipulator.h"
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
			CardManipulator(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref) :
				state_(state), flow_context_(flow_context), card_ref_(card_ref)
			{
			}

			void Cost(int new_cost) { GetCard().SetCost(new_cost); }
			void Attack(int new_attack) { GetCard().SetAttack(new_attack); }
			void MaxHP(int new_max_hp) { GetCard().SetMaxHP(new_max_hp); }

			// TODO: only accessible by enchantment helper?
			void SpellDamage(int v) { GetCard().SetSpellDamage(v); }

			detail::DamageSetter Internal_SetDamage() { return detail::DamageSetter(state_, card_ref_); }

		public:
			void Damage(state::CardRef source, int amount) {
				int final_amount = 0;
				BoardManipulator(state_, flow_context_)
					.CalculateFinalDamageAmount(source, amount, &final_amount);
				Helpers::DamageHelper(state_, flow_context_,
					source, card_ref_, amount).ConductDamage(final_amount);
			}
			void Heal(state::CardRef source, int amount) {
				return Damage(source, -amount);
			}

			void ConductFinalDamage(state::CardRef source, int amount) {
				auto helper = Helpers::DamageHelper(state_, flow_context_,
					source, card_ref_, amount);
				helper.ConductDamage(amount);
			}

			void SetPlayOrder() {
				GetCard().SetPlayOrder(state_.GetPlayOrder());
			}

			Helpers::EnchantmentHelper Enchant() { return Helpers::EnchantmentHelper(state_, flow_context_, card_ref_); }

		public:
			void AfterAddedToPlayZone(state::Events::Manager & event_mgr);

		protected:
			state::Cards::Card & GetCard();

		protected:
			state::State & state_;
			FlowControl::FlowContext & flow_context_;
			state::CardRef card_ref_;
		};
	}
}