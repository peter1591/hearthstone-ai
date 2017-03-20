#pragma once

#include "state/Types.h"
#include "state/State.h"
#include "FlowControl/Manipulators/BoardManipulator.h"

namespace FlowControl
{
	namespace Manipulators
	{
		inline void BoardManipulator::Summon(state::Cards::CardData && card_data, state::PlayerIdentifier player, int pos)
		{
			assert(player.AssertCheck());

			card_data.enchanted_states.player = player;
			assert(card_data.card_type == state::kCardTypeMinion);
			card_data.enchantment_handler.SetOriginalStates(card_data.enchanted_states);

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

			state_.TriggerEvent<state::Events::EventTypes::AfterMinionSummoned>();
			Manipulate(state_, flow_context_).Minion(ref).AfterSummoned();
		}

		inline int BoardManipulator::GetSpellDamage(state::PlayerIdentifier player_id)
		{
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
	}
}