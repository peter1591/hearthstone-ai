#pragma once

#include "Entity/AuraAuxData.h"
#include "EntitiesManager/EntitiesManager.h"

namespace Manipulators
{
	namespace Helpers
	{
		AuraHelper::AuraHelper(EntitiesManager& mgr, Entity::AuraAuxData& data) : mgr_(mgr), data_(data) {}

		template <typename ClientAuraHelper_>
		void AuraHelper::Update(ClientAuraHelper_&& client_aura_helper)
		{
			using ClientAuraHelper = std::decay_t<ClientAuraHelper_>;
			using EnchantmentType = ClientAuraHelper::EnchantmentType;

			for (auto it = data_.applied_enchantments.begin(), it2 = data_.applied_enchantments.end(); it != it2;)
			{
				if (client_aura_helper.IsEligible(it->first)) {
					++it;
				}
				else {
					mgr_.GetMinionManipulator(it->first).GetEnchantmentHelper()
						.Remove<EnchantmentType>(it->second);
					it = data_.applied_enchantments.erase(it);
				}
			}

			client_aura_helper.IterateEligibles([this, &client_aura_helper](CardRef target) {
				if (data_.applied_enchantments.find(target) != data_.applied_enchantments.end()) return; // already applied

				auto enchant_identifier = mgr_.GetMinionManipulator(target).GetEnchantmentHelper()
					.Add<EnchantmentType>(client_aura_helper.CreateEnchantmentFor(target));
				data_.applied_enchantments.insert(std::make_pair(target, std::move(enchant_identifier)));
			});
		}
	}
}