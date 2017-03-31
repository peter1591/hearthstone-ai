#pragma once

#include "state/Cards/Card.h"
#include "state/ZoneChanger.h"
#include "FlowControl/Manipulators/BoardManipulator.h"
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
			void Damage(state::CardRef source, int amount) {
				return Damage(source, state_.GetCard(source), amount);
			}
			void Damage(state::CardRef source, state::Cards::Card const& source_card, int amount) {
				int final_amount = 0;
				BoardManipulator(state_, flow_context_)
					.CalculateFinalDamageAmount(source, source_card, amount, &final_amount);
				Helpers::DamageHelper(state_, flow_context_,
					source, source_card,
					card_ref_, card_, amount).ConductDamage(final_amount);
			}
			void Heal(state::CardRef source, int amount) {
				return Heal(source, state_.GetCard(source), amount);
			}
			void Heal(state::CardRef source, state::Cards::Card const& source_card, int amount) {
				return Damage(source, source_card, -amount);
			}

			void ConductFinalDamage(state::CardRef source, state::Cards::Card const& source_card, int amount) {
				auto helper = Helpers::DamageHelper(state_, flow_context_,
					source, source_card,
					card_ref_, card_, amount);
				helper.ConductDamage(amount);
			}
			void ConductFinalDamage(state::CardRef source, int amount) {
				return ConductFinalDamage(source, state_.GetCard(source), amount);
			}

			Helpers::EnchantmentHelper Enchant() { return Helpers::EnchantmentHelper(state_, flow_context_, card_ref_, card_); }
			Helpers::DeathrattlesHelper Deathrattles() { return Helpers::DeathrattlesHelper(state_, flow_context_, card_ref_, card_); }

		public:
			void AfterAddedToPlayZone(state::Events::Manager & event_mgr);

		protected:
			state::State & state_;
			FlowControl::FlowContext & flow_context_;
			state::CardRef card_ref_;
			state::Cards::Card & card_;
		};
	}
}