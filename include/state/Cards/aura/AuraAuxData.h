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
				AuraAuxData() : last_update_hint_id(0), removed1(false) {}

				bool Empty() const { return applied_enchantments.empty(); }

				std::unordered_map<CardRef, TieredEnchantments::ContainerType::Identifier> applied_enchantments;
				int last_update_hint_id;
				bool removed1;
			};
		}
	}
}