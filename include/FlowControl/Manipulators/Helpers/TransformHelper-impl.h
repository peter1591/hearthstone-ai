#pragma once

#include "FlowControl/Manipulators/Helpers/TransformHelper.h"
#include "state/State.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			inline void TransformHelper::Transform(state::Cards::CardData const& new_data)
			{
				assert(card_.GetZone() == new_data.zone);
				state::PlayerIdentifier final_player = card_.GetPlayerIdentifier();

				state::Cards::CardData final_data = new_data;

				final_data.play_order = card_.GetRawData().play_order;
				final_data.enchanted_states.player = card_.GetRawData().enchanted_states.player;
				final_data.zone = card_.GetRawData().zone;
				final_data.just_played = card_.GetRawData().just_played;

				final_data.enchantment_handler.AfterTransformCopy(final_data, final_player);
				final_data.aura_handler.AfterTransformCopy();
				final_data.flag_aura_handler.AfterTransformCopy();

				card_.TransformByCopy(new_data);
			}
		}
	}
}