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
			RawCard() :
				card_id(-1), card_type(kCardTypeInvalid), play_order(-1), zone_position(-1), damaged(0)
			{
			}

			int card_id;
			CardType card_type;
			int play_order;
			int zone_position;

			int damaged;
			EnchantableStates enchanted_states;

			EnchantmentAuxData enchantment_aux_data;
			AuraAuxData aura_aux_data;

		public: // for hero type
			CardRef weapon_ref;
		};
	}
}