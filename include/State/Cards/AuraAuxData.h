#pragma once

#include <unordered_map>
#include "State/Types.h"
#include "Enchantments/TieredEnchantments.h"

namespace state
{
	namespace Cards
	{
		class AuraAuxData
		{
		public:
			AuraAuxData() : last_update_hint_id(0), removed(false) {}

			bool Empty() const { return applied_enchantments.empty(); }

			std::unordered_map<CardRef, TieredEnchantments::ContainerType::Identifier> applied_enchantments;
			int last_update_hint_id;
			bool removed;
		};
	}
}