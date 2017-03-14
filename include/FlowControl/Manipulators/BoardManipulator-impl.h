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

			card_data.zone = state::kCardZoneInvalid;
			card_data.enchantable_states.player = player;
			assert(card_data.card_type == state::kCardTypeMinion);
			card_data.enchantment_aux_data.origin_states = card_data.enchantable_states;

			state::CardRef ref = state_.AddCard(state::Cards::Card(std::move(card_data)));

			state_.GetZoneChanger<state::kCardZoneInvalid, state::kCardTypeMinion>(flow_context_.random_, ref)
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
	}
}