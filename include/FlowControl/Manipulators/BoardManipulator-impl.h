#pragma once

#include "state/Types.h"
#include "state/State.h"
#include "Cards/CardDispatcher.h"
#include "FlowControl/Manipulators/BoardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline state::Cards::CardData BoardManipulator::GenerateCard(Cards::CardId card_id, state::PlayerIdentifier player)
		{
			state::Cards::CardData card_data = Cards::CardDispatcher::CreateInstance(card_id);
			card_data.enchanted_states.player = player;
			card_data.enchantment_handler.SetOriginalStates(card_data.enchanted_states);
			// TODO: set play order

			return std::move(card_data);
		}

		inline void BoardManipulator::SummonMinion(state::Cards::CardData && card_data, int pos)
		{
			state::PlayerIdentifier player = card_data.enchanted_states.player;
			assert(player.AssertCheck());

			assert(card_data.card_type == state::kCardTypeMinion);

			assert(((card_data.zone = state::kCardZoneNewlyCreated), true)); // assign it just for debug assertion
			state::CardRef ref = state_.AddCard(state::Cards::Card(std::move(card_data)));

			state_.GetZoneChanger<state::kCardZoneNewlyCreated, state::kCardTypeMinion>(flow_context_.GetRandom(), ref)
				.ChangeTo<state::kCardZonePlay>(player, pos);

			state::Cards::Card const& card = state_.GetCardsManager().Get(ref);
			assert(card.GetPlayerIdentifier() == player);
			assert(card.GetZone() == state::kCardZonePlay);
			assert(card.GetCardType() == state::kCardTypeMinion);
			assert(card.GetZonePosition() == pos);

			state_.TriggerEvent<state::Events::EventTypes::BeforeMinionSummoned>(
				state::Events::EventTypes::BeforeMinionSummoned::Context{ state_, ref, card });

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
			{ state_, flow_context_, source, source_card, final_amount });
		}
	}
}