#pragma once

#include <string>
#include "State/Types.h"
#include "State/Cards/EnchantableStates.h"
#include "State/Cards/EnchantmentAuxData.h"
#include "State/Cards/AuraAuxData.h"

namespace state
{
	namespace Cards
	{
		class RawCard
		{
		public:
			RawCard() : card_type(kCardTypeInvalid), play_order(-1), damaged(0) {}

			CardType card_type;
			int card_id;
			int play_order;
			int zone_position;

			int damaged;
			EnchantableStates enchanted_states;

			EnchantmentAuxData enchantment_aux_data;
			AuraAuxData aura_aux_data;
		};
	}
}