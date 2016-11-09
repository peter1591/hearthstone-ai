#pragma once

#include <string>
#include "Entity/CardType.h"
#include "Entity/CardZone.h"
#include "Entity/EnchantableStates.h"
#include "Entity/EnchantmentAuxData.h"

namespace Entity
{
	struct RawCard
	{
		CardType card_type;
		std::string card_id;

		int play_order;
		int damaged;
		EnchantableStates enchanted_states;

		EnchantmentAuxData enchantment_aux_data;
	};
}