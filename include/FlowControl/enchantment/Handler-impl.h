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
			assert(GetCard().GetRawData().enchanted_states == new_states);

			int hp = GetCard().GetHP();
			if (!allow_hp_reduce && hp < origin_hp && hp != GetCard().GetMaxHP()) {
				assert(GetCard().GetMaxHP() > 0);
				int target_hp = std::min(GetCard().GetMaxHP(), origin_hp);

				// do not trigger healing events
				Manipulate(state, flow_context).OnBoardMinion(card_ref)
					.Internal_SetDamage().Heal(target_hp - hp);

				hp = GetCard().GetHP();
				assert(hp == target_hp);
			}

			if (origin_hp > 0 && hp <= 0) {
				flow_context.AddDeadEntryHint(state, card_ref);
			}

			enchantments.FinishedUpdate(state);
		}

		inline void Handler::UpdateCard(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const & new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 15, "enchantable fields changed");
			auto GetCard = [&]() { return state.GetCard(card_ref); };

			state::Cards::EnchantableStates const& current_states = GetCard().GetRawData().enchanted_states;

			auto card_manipulator = Manipulators::CardManipulator(state, flow_context, card_ref);

			if (new_states.cost != current_states.cost) {
				card_manipulator.Cost(new_states.cost);
				assert(GetCard().GetCost() == new_states.cost);
			}

			if (new_states.cost_health_instead != current_states.cost_health_instead) {
				card_manipulator.CostHealthInstead(new_states.cost_health_instead);
				assert(GetCard().GetCostHealthInstead() == new_states.cost_health_instead);
			}
		}

		inline void Handler::UpdateCharacter(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 15, "enchantable fields changed");
			auto GetCard = [&]() { return state.GetCard(card_ref); };

			state::Cards::EnchantableStates const& current_states = GetCard().GetRawData().enchanted_states;

			UpdateCard(state, flow_context, card_ref, new_states);

			auto character_manipulator = Manipulators::CharacterManipulator(state, flow_context, card_ref);

			if (new_states.attack != current_states.attack) {
				character_manipulator.Attack(new_states.attack);
				assert(GetCard().GetAttack() == new_states.attack);
			}
			if (new_states.max_hp != current_states.max_hp) {
				character_manipulator.MaxHP(new_states.max_hp);
				assert(GetCard().GetMaxHP() == new_states.max_hp);
			}
			if (new_states.spell_damage != current_states.spell_damage) {
				character_manipulator.SpellDamage(new_states.spell_damage);
				assert(GetCard().GetSpellDamage() == new_states.spell_damage);
			}
			if (new_states.max_attacks_per_turn != current_states.max_attacks_per_turn) {
				state.GetMutableCard(card_ref).SetMaxAttacksPerTurn(new_states.max_attacks_per_turn);
				assert(GetCard().GetMaxAttacksPerTurn() == new_states.max_attacks_per_turn);
			}
			if (new_states.immune_to_spell != current_states.immune_to_spell) {
				state.GetMutableCard(card_ref).SetImmuneToSpell(new_states.immune_to_spell);
				assert(GetCard().IsImmuneToSpell() == new_states.immune_to_spell);
			}
			if (new_states.poisonous != current_states.poisonous) {
				state.GetMutableCard(card_ref).SetPoisonous(new_states.poisonous);
				assert(GetCard().IsPoisonous() == new_states.poisonous);
			}
			if (new_states.freeze_attack != current_states.freeze_attack) {
				state.GetMutableCard(card_ref).SetFreezeAttack(new_states.freeze_attack);
				assert(GetCard().IsFreezeAttack() == new_states.freeze_attack);
			}
			if (new_states.cant_attack_hero != current_states.cant_attack_hero) {
				state.GetMutableCard(card_ref).SetCantAttackHero(new_states.cant_attack_hero);
				assert(GetCard().IsCantAttackHero() == new_states.cant_attack_hero);
			}
		}

		inline void Handler::UpdateMinion(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 15, "enchantable fields changed");
			auto GetCard = [&]() { return state.GetCard(card_ref); };
			state::Cards::EnchantableStates const& current_states = GetCard().GetRawData().enchanted_states;

			UpdateCharacter(state, flow_context, card_ref, new_states);

			assert(GetCard().GetZone() == state::kCardZonePlay);
			assert(GetCard().GetCardType() == state::kCardTypeMinion);
			auto manipulator = Manipulators::OnBoardMinionManipulator(state, flow_context, card_ref);
			if (new_states.player != current_states.player) {
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

			if (new_states.charge != current_states.charge) {
				manipulator.Charge(new_states.charge);
			}

			if (new_states.stealth != current_states.stealth) {
				manipulator.Stealth(new_states.stealth);
			}
		}

		inline void Handler::UpdateHero(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 15, "enchantable fields changed");
			UpdateCharacter(state, flow_context, card_ref, new_states);
		}

		inline void Handler::UpdateWeapon(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates const& new_states)
		{
			static_assert(state::Cards::EnchantableStates::kFieldChangeId == 15, "enchantable fields changed");
			auto GetCard = [&]() { return state.GetCard(card_ref); };
			state::Cards::EnchantableStates const& current_states = GetCard().GetRawData().enchanted_states;

			UpdateCard(state, flow_context, card_ref, new_states);

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
			if (new_states.poisonous != current_states.poisonous) {
				state.GetMutableCard(card_ref).SetPoisonous(new_states.poisonous);
				assert(GetCard().IsPoisonous() == new_states.poisonous);
			}
			if (new_states.freeze_attack != current_states.freeze_attack) {
				state.GetMutableCard(card_ref).SetFreezeAttack(new_states.freeze_attack);
				assert(GetCard().IsFreezeAttack() == new_states.freeze_attack);
			}
			if (new_states.cant_attack_hero != current_states.cant_attack_hero) {
				state.GetMutableCard(card_ref).SetCantAttackHero(new_states.cant_attack_hero);
				assert(GetCard().IsCantAttackHero() == new_states.cant_attack_hero);
			}
		}
	};
}
