#pragma once

#include "FlowControl/Manipulators/Helpers/TransformHelper.h"
#include "state/State.h"
#include "Cards/CardDispatcher.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline void TransformHelper::Transform(Cards::CardId id)
			{
				state::Cards::CardData new_data = Cards::CardDispatcher::CreateInstance(id);

				new_data.enchanted_states.player = card_.GetPlayerIdentifier();
				assert(new_data.card_type == state::kCardTypeMinion);
				new_data.enchantment_handler.SetOriginalStates(new_data.enchanted_states);
				
				new_data.zone = card_.GetZone();

				BecomeCopyOf(new_data);
			}

			inline void TransformHelper::BecomeCopyOf(state::Cards::CardData const& new_data)
			{
				card_.GetMutableAuraHandlerGetter().Get().BeforeTransform(state_, flow_context_);
				card_.GetMutableFlagAuraHandlerGetter().Get().BeforeTransform(state_, flow_context_, card_ref_, card_);

				assert(card_.GetRawData().aura_handler.NoAppliedEnchantment());
				assert(card_.GetRawData().flag_aura_handler.NoAppliedEffect());

				assert(card_.GetZone() == new_data.zone);
				state::Cards::CardData final_data = new_data;


				final_data.enchantment_handler.AfterTransformCopy(final_data, card_.GetPlayerIdentifier());
				final_data.aura_handler.AfterTransformCopy();
				final_data.flag_aura_handler.AfterTransformCopy();

				final_data.zone = state::CardZone::kCardZoneNewlyCreated;
				state::CardRef new_card_ref = state_.AddCard(state::Cards::Card(std::move(final_data)));

				// these fields should not be changed
				final_data.play_order = card_.GetRawData().play_order;
				final_data.enchanted_states.player = card_.GetRawData().enchanted_states.player;
				final_data.zone = card_.GetRawData().zone;
				final_data.zone_position = card_.GetRawData().zone_position;
				final_data.just_played = card_.GetRawData().just_played;

				state_.ReplaceCard(card_ref_, new_card_ref);

				//card_.TransformByCopy(final_data);

				//state_.GetBoard().Get(card_.GetPlayerIdentifier()).minions_.IncreaseChangeId();
			}
		}
	}
}