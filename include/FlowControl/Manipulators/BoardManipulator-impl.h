#pragma once

#include "state/Types.h"
#include "state/State.h"
#include "Cards/CardDispatcher.h"
#include "FlowControl/Manipulators/BoardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline state::Cards::Card BoardManipulator::GenerateCardById(Cards::CardId card_id, state::PlayerIdentifier player)
		{
			state::Cards::CardData new_data = Cards::CardDispatcher::CreateInstance(card_id);
			new_data.enchanted_states.player = player;
			new_data.enchantment_handler.SetOriginalStates(new_data.enchanted_states);
			return GenerateCard(std::move(new_data), player);
		}

		inline state::Cards::Card BoardManipulator::GenerateCardByCopy(state::Cards::Card const & card, state::PlayerIdentifier player)
		{
			state::Cards::CardData new_data = card.GetRawData();

			// TODO: remove enchantments which are listening to an event

			new_data.enchantment_handler.Aura().AfterCopied();
			new_data.aura_handler.AfterCopied();
			new_data.flag_aura_handler.AfterCopied();

			if (new_data.enchanted_states.player != player) {
				auto origin_states = new_data.enchantment_handler.GetOriginalStates();
				origin_states.player = player;
				new_data.enchantment_handler.SetOriginalStates(origin_states);
				new_data.enchanted_states.player = player;
			}

			return GenerateCard(std::move(new_data), player);
		}

		inline state::Cards::Card BoardManipulator::GenerateCard(state::Cards::CardData card_data, state::PlayerIdentifier player)
		{
			// TODO: set play order

			assert(((card_data.zone = state::kCardZoneNewlyCreated), true)); // assign it just for debug assertion

			return state::Cards::Card(std::move(card_data));
		}

		inline void BoardManipulator::SummonMinion(state::Cards::Card origin_card, int pos)
		{
			state::PlayerIdentifier player = origin_card.GetPlayerIdentifier();
			assert(player.AssertCheck());

			assert(origin_card.GetCardType() == state::kCardTypeMinion);

			state::CardRef ref = state_.AddCard(std::move(origin_card));

			state_.GetZoneChanger<state::kCardZoneNewlyCreated, state::kCardTypeMinion>(Manipulate(state_, flow_context_), ref)
				.ChangeTo<state::kCardZonePlay>(player, pos);

			state::Cards::Card const& card = state_.GetCardsManager().Get(ref);
			assert(card.GetPlayerIdentifier() == player);
			assert(card.GetZone() == state::kCardZonePlay);
			assert(card.GetCardType() == state::kCardTypeMinion);
			assert(card.GetZonePosition() == pos);

			state_.TriggerEvent<state::Events::EventTypes::BeforeMinionSummoned>(
				state::Events::EventTypes::BeforeMinionSummoned::Context{ Manipulate(state_, flow_context_), ref, card });

			state_.TriggerEvent<state::Events::EventTypes::AfterMinionSummoned>(ref, card);
			Manipulate(state_, flow_context_).Minion(ref).AfterSummoned();
		}

		inline int BoardManipulator::GetSpellDamage(state::PlayerIdentifier player_id)
		{
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

		inline void BoardManipulator::CalculateFinalDamageAmount(state::CardRef source, state::Cards::Card const & source_card, int amount, int * final_amount)
		{
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
			{ Manipulate(state_, flow_context_), source, source_card, final_amount });
		}
	}
}