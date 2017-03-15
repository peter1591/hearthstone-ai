#pragma once

#include <unordered_map>
#include "state/Types.h"
#include "state/Cards/Enchantments/TieredEnchantments.h"

namespace state {
	namespace Cards {
		namespace aura {
			class AuraAuxData
			{
			public:
				AuraAuxData() :
					last_updated_change_id_first_player_minions_(-1), // ensure this is not the initial value of the actual change id
					last_updated_change_id_second_player_minions_(-1)
				{}

				bool Empty() const { return applied_enchantments.empty(); }

				std::unordered_map<CardRef, TieredEnchantments::ContainerType::Identifier> applied_enchantments;

				int last_updated_change_id_first_player_minions_;
				int last_updated_change_id_second_player_minions_;
			};
		}
	}
}