#pragma once

#include <string>
#include "Entity/CardType.h"
#include "Entity/CardZone.h"
#include "Entity/EnchantableStates.h"
#include "Entity/EnchantmentAuxData.h"
#include "Entity/AuraAuxData.h"

namespace Entity
{
	class RawCard
	{
	public:
		RawCard() : card_type(kCardTypeInvalid), play_order(-1), damaged(0) {}

		CardType card_type;
		std::string card_id;
		int play_order;
		int zone_position;

		int damaged;
		EnchantableStates enchanted_states;

		EnchantmentAuxData enchantment_aux_data;
		AuraAuxData aura_aux_data;
	};
}