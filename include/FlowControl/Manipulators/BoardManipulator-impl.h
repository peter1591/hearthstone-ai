#pragma once

#include "state/Types.h"
#include "state/State.h"
#include "Cards/CardDispatcher.h"
#include "FlowControl/Manipulators/BoardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline state::CardRef BoardManipulator::AddCardById(Cards::CardId card_id, state::PlayerIdentifier player)
		{
			state::Cards::CardData new_data = Cards::CardDispatcher::CreateInstance(card_id);
			new_data.enchanted_states.player = player;
			new_data.enchantment_handler.SetOriginalStates(new_data.enchanted_states);

			return state_.AddCard(GenerateCard(std::move(new_data), player));
		}

		inline state::CardRef BoardManipulator::AddCardByCopy(state::Cards::Card const & card, state::PlayerIdentifier player)
		{
			state::Cards::CardData new_data = card.GetRawData();

			if (new_data.enchanted_states.player != player) {
				auto origin_states = new_data.enchantment_handler.GetOriginalStates();
				origin_states.player = player;
				new_data.enchantment_handler.SetOriginalStates(origin_states);
				new_data.enchanted_states.player = player;
			}

			state::CardRef card_ref = state_.AddCard(GenerateCard(std::move(new_data), player));

			state_.GetMutableCard(card_ref).GetMutableEnchantmentHandler()
				.AfterCopied(FlowControl::Manipulate(state_, flow_context_), card_ref);

			return card_ref;
		}

		inline state::Cards::Card BoardManipulator::GenerateCard(state::Cards::CardData card_data, state::PlayerIdentifier player)
		{
			// TODO: set play order

			assert(((card_data.zone = state::kCardZoneNewlyCreated), true)); // assign it just for debug assertion

			return state::Cards::Card(std::move(card_data));
		}

		inline state::CardRef BoardManipulator::SummonMinion(state::CardRef card_ref, int pos)
		{
			state::PlayerIdentifier player = state_.GetCard(card_ref).GetPlayerIdentifier();
			assert(player.AssertCheck());
			assert(state_.GetCard(card_ref).GetCardType() == state::kCardTypeMinion);

			state_.GetZoneChanger<state::kCardZoneNewlyCreated, state::kCardTypeMinion>(Manipulate(state_, flow_context_), card_ref)
				.ChangeTo<state::kCardZonePlay>(player, pos);

			state::Cards::Card const& card = state_.GetCardsManager().Get(card_ref);
			assert(card.GetPlayerIdentifier() == player);
			assert(card.GetZone() == state::kCardZonePlay);
			assert(card.GetCardType() == state::kCardTypeMinion);
			assert(card.GetZonePosition() == pos);

			state_.TriggerEvent<state::Events::EventTypes::BeforeMinionSummoned>(
				state::Events::EventTypes::BeforeMinionSummoned::Context{ Manipulate(state_, flow_context_), card_ref });

			state_.TriggerEvent<state::Events::EventTypes::AfterMinionSummoned>(
				state::Events::EventTypes::AfterMinionSummoned::Context{ Manipulate(state_, flow_context_), card_ref });

			Manipulate(state_, flow_context_).OnBoardMinion(card_ref).AfterSummoned();

			return card_ref;
		}

		inline int BoardManipulator::GetSpellDamage(state::PlayerIdentifier player_id)
		{
			// TODO: add a cached value, and only update when board update
			assert(player_id.IsValid());
			state::board::Player const& player = state_.GetBoard().Get(player_id);

			int spell_damage = 0;

			spell_damage += state_.GetCardsManager().Get(player.GetHeroRef()).GetSpellDamage();

			auto weapon_ref = player.GetWeaponRef();
			if (weapon_ref.IsValid()) spell_damage += state_.GetCardsManager().Get(weapon_ref).GetSpellDamage();

			for (state::CardRef const& minion_ref : player.minions_.Get()) {
				spell_damage += state_.GetCardsManager().Get(minion_ref).GetSpellDamage();
			}

			return spell_damage;
		}

		inline void BoardManipulator::CalculateFinalDamageAmount(state::CardRef source, int amount, int * final_amount)
		{
			state::Cards::Card const & source_card = state_.GetCard(source);
			*final_amount = amount;
			
			if (amount > 0) { // spell damage only acts on damages, not healings
				if (source_card.GetCardType() == state::kCardTypeSpell ||
					source_card.GetCardType() == state::kCardTypeSecret)
				{
					*final_amount += GetSpellDamage(source_card.GetPlayerIdentifier());
				}
			}

			state_.TriggerEvent<state::Events::EventTypes::CalculateHealDamageAmount>(
				state::Events::EventTypes::CalculateHealDamageAmount::Context
			{ Manipulate(state_, flow_context_), source, final_amount });
		}
	}
}