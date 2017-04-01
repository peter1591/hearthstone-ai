#pragma once

#include "FlowControl/enchantment/Handler.h"

#include "state/State.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/Manipulate.h"

#include "FlowControl/enchantment/Enchantments-impl.h"

namespace FlowControl
{
	namespace enchantment
	{
		inline void Handler::Update(state::State & state, FlowContext & flow_context, state::CardRef card_ref, bool allow_hp_reduce) {
			if (!enchantments.NeedUpdate(state)) return;

			auto GetCard = [&]() { return state.GetCard(card_ref); };

			int origin_hp = GetCard().GetHP();

			state::Cards::EnchantableStates const& current_states = GetCard().GetRawData().enchanted_states;
			state::Cards::EnchantableStates new_states = origin_states;
			enchantments.ApplyAll(state, new_states);

			switch (GetCard().GetCardType()) {
			case state::kCardTypeHero:
				UpdateHero(state, flow_context, card_ref, new_states);
				break;
			case state::kCardTypeMinion:
				UpdateMinion(state, flow_context, card_ref, new_states);
				break;
			case state::kCardTypeWeapon:
				UpdateWeapon(state, flow_context, card_ref, new_states);
				break;
			default:
				throw std::exception("not implemented");
			}

			int hp = GetCard().GetHP();
			if (!allow_hp_reduce && hp < origin_hp && hp != GetCard().GetMaxHP()) {
				assert(GetCard().GetMaxHP() > 0);
				int target_hp = std::min(GetCard().GetMaxHP(), origin_hp);

				// do not trigger healing events
				Manipulate(state, flow_context).Minion(card_ref)
					.Internal_SetDamage().Heal(target_hp - hp);

				hp = GetCard().GetHP();
				assert(hp == target_hp);
			}

			if (origin_hp > 0 && hp <= 0) {
				flow_context.AddDeadEntryHint(state, card_ref);
			}

			enchantments.FinishedUpdate(state);
		}

		inline void Handler::UpdateCharacter(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 6, "enchantable fields changed");
			auto GetCard = [&]() { return state.GetCard(card_ref); };

			state::Cards::EnchantableStates const& current_states = GetCard().GetRawData().enchanted_states;

			auto card_manipulator = Manipulators::CardManipulator(state, flow_context, card_ref);

			if (new_states.attack != current_states.attack) {
				card_manipulator.Attack(new_states.attack);
				assert(GetCard().GetAttack() == new_states.attack);
			}
			if (new_states.max_hp != current_states.max_hp) {
				card_manipulator.MaxHP(new_states.max_hp);
				assert(GetCard().GetMaxHP() == new_states.max_hp);
			}
			if (new_states.spell_damage != current_states.spell_damage) {
				card_manipulator.SpellDamage(new_states.spell_damage);
				assert(GetCard().GetSpellDamage() == new_states.spell_damage);
			}
		}

		inline void Handler::UpdateMinion(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 6, "enchantable fields changed");
			auto GetCard = [&]() { return state.GetCard(card_ref); };
			state::Cards::EnchantableStates const& current_states = GetCard().GetRawData().enchanted_states;

			UpdateCharacter(state, flow_context, card_ref, new_states);

			assert(GetCard().GetCardType() == state::kCardTypeMinion);
			auto manipulator = Manipulators::MinionManipulator(state, flow_context, card_ref);
			if (new_states.player != current_states.player) {
				assert(GetCard().GetZone() == state::kCardZonePlay);

				if (state.GetBoard().Get(new_states.player).minions_.Full()) {
					state.GetZoneChanger<state::kCardTypeMinion, state::kCardZonePlay>(Manipulate(state, flow_context), card_ref)
						.ChangeTo<state::kCardZoneGraveyard>(current_states.player);
					assert(GetCard().GetZone() == state::kCardZoneGraveyard);
					assert(GetCard().GetPlayerIdentifier() == current_states.player);
				}
				else {
					int location = (int)state.GetBoard().Get(new_states.player).minions_.Size();

					state.GetZoneChanger<state::kCardTypeMinion, state::kCardZonePlay>(Manipulate(state, flow_context), card_ref)
						.ChangeTo<state::kCardZonePlay>(new_states.player, location);
					assert(GetCard().GetZone() == state::kCardZonePlay);
					assert(GetCard().GetPlayerIdentifier() == new_states.player);
				}
			}

			if (new_states.cost != current_states.cost) {
				manipulator.Cost(new_states.cost);
				assert(GetCard().GetCost() == new_states.cost);
			}

			assert(GetCard().GetRawData().enchanted_states == new_states);
		}

		inline void Handler::UpdateHero(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 6, "enchantable fields changed");
			UpdateCharacter(state, flow_context, card_ref, new_states);
		}

		inline void Handler::UpdateWeapon(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 6, "enchantable fields changed");
			auto GetCard = [&]() { return state.GetCard(card_ref); };
			state::Cards::EnchantableStates const& current_states = GetCard().GetRawData().enchanted_states;

			auto manipulator = Manipulators::WeaponManipulator(state, flow_context, card_ref);

			if (new_states.attack != current_states.attack) {
				manipulator.Attack(new_states.attack);
				assert(GetCard().GetAttack() == new_states.attack);
			}
			if (new_states.max_hp != current_states.max_hp) {
				manipulator.MaxHP(new_states.max_hp);
				assert(GetCard().GetMaxHP() == new_states.max_hp);
			}
			if (new_states.spell_damage != current_states.spell_damage) {
				manipulator.SpellDamage(new_states.spell_damage);
				assert(GetCard().GetSpellDamage() == new_states.spell_damage);
			}

			assert(GetCard().GetRawData().enchanted_states == new_states);
		}
	};
}
