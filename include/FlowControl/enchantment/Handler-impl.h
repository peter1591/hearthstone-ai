#pragma once

#include "FlowControl/enchantment/Handler.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace enchantment
	{
		inline void Handler::Update(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
			if (!need_update) return;

			state::Cards::EnchantableStates const& current_states = card.GetRawData().enchanted_states;
			state::Cards::EnchantableStates new_states = origin_states;
			enchantments.ApplyAll(new_states);

			switch (card.GetCardType()) {
			case state::kCardTypeHero:
				UpdateHero(state, flow_context, card_ref, card, new_states);
				break;
			case state::kCardTypeMinion:
				UpdateMinion(state, flow_context, card_ref, card, new_states);
				break;
			case state::kCardTypeWeapon:
				UpdateWeapon(state, flow_context, card_ref, card, new_states);
				break;
			default:
				throw std::exception("not implemented");
			}

			// removing enchantments should not kill a minion
			int hp = card.GetHP();
			if (hp <= 0) {
				// Do not trigger healing events
				Manipulators::MinionManipulator(state, flow_context, card_ref, card)
					.Internal_SetDamage().Heal(-hp + 1);
				assert(card.GetHP() == 1);
			}

			need_update = false;
		}

		inline void Handler::UpdateCharacter(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 5, "enchantable fields changed");

			state::Cards::EnchantableStates const& current_states = card.GetRawData().enchanted_states;

			auto card_manipulator = Manipulators::CardManipulator(state, flow_context, card_ref, card);

			if (new_states.attack != current_states.attack) {
				card_manipulator.Attack(new_states.attack);
				assert(card.GetAttack() == new_states.attack);
			}
			if (new_states.max_hp != current_states.max_hp) {
				card_manipulator.MaxHP(new_states.max_hp);
				assert(card.GetMaxHP() == new_states.max_hp);
			}
			if (new_states.spell_damage != current_states.spell_damage) {
				card_manipulator.SpellDamage(new_states.spell_damage);
				assert(card.GetSpellDamage() == new_states.spell_damage);
			}
		}

		inline void Handler::UpdateMinion(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 5, "enchantable fields changed");
			state::Cards::EnchantableStates const& current_states = card.GetRawData().enchanted_states;

			UpdateCharacter(state, flow_context, card_ref, card, new_states);

			assert(card.GetCardType() == state::kCardTypeMinion);
			auto manipulator = Manipulators::MinionManipulator(state, flow_context, card_ref, card);
			if (new_states.player != current_states.player) {
				assert(card.GetZone() == state::kCardZonePlay);

				if (state.GetBoard().Get(new_states.player).minions_.Full()) {
					state.GetZoneChanger<state::kCardTypeMinion, state::kCardZonePlay>(flow_context.GetRandom(), card_ref)
						.ChangeTo<state::kCardZoneGraveyard>(current_states.player);
					assert(card.GetZone() == state::kCardZoneGraveyard);
					assert(card.GetPlayerIdentifier() == current_states.player);
				}
				else {
					int location = (int)state.GetBoard().Get(new_states.player).minions_.Size();

					state.GetZoneChanger<state::kCardTypeMinion, state::kCardZonePlay>(flow_context.GetRandom(), card_ref)
						.ChangeTo<state::kCardZonePlay>(new_states.player, location);
					assert(card.GetZone() == state::kCardZonePlay);
					assert(card.GetPlayerIdentifier() == new_states.player);
				}
			}

			if (new_states.cost != current_states.cost) {
				manipulator.Cost(new_states.cost);
				assert(card.GetCost() == new_states.cost);
			}

			if (new_states.taunt != current_states.taunt) {
				manipulator.Taunt(new_states.taunt);
				assert(card.GetRawData().enchanted_states.taunt == new_states.taunt);
			}

			if (new_states.shielded != current_states.shielded) {
				manipulator.Shield(new_states.shielded);
				assert(card.GetRawData().enchanted_states.shielded == new_states.shielded);
			}

			if (new_states.charge != current_states.charge) {
				manipulator.Charge(new_states.charge);
				assert(card.GetRawData().enchanted_states.charge == new_states.charge);
			}

			assert(card.GetRawData().enchanted_states == new_states);
		}

		inline void Handler::UpdateHero(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 5, "enchantable fields changed");
			UpdateCharacter(state, flow_context, card_ref, card, new_states);
		}

		inline void Handler::UpdateWeapon(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 5, "enchantable fields changed");
			state::Cards::EnchantableStates const& current_states = card.GetRawData().enchanted_states;

			auto manipulator = Manipulators::WeaponManipulator(state, flow_context, card_ref, card);

			if (new_states.attack != current_states.attack) {
				manipulator.Attack(new_states.attack);
				assert(card.GetAttack() == new_states.attack);
			}
			if (new_states.max_hp != current_states.max_hp) {
				manipulator.MaxHP(new_states.max_hp);
				assert(card.GetMaxHP() == new_states.max_hp);
			}
			if (new_states.spell_damage != current_states.spell_damage) {
				manipulator.SpellDamage(new_states.spell_damage);
				assert(card.GetSpellDamage() == new_states.spell_damage);
			}

			assert(card.GetRawData().enchanted_states == new_states);
		}
	};
}
