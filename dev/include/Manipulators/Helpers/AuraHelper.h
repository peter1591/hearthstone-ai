#pragma once

#include "Entity/AuraAuxData.h"

class EntitiesManager;

namespace Manipulators
{
	namespace Helpers
	{
		class AuraHelper
		{
		public:
			AuraHelper(EntitiesManager& mgr, Entity::Card & card) :
				mgr_(mgr),
				data_(card.GetMutableAuraAuxDataGetter().Get())
			{
			}

			template <typename ClientAuraHelper_>
			void Update(ClientAuraHelper_&& client_aura_helper)
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
						.Add(client_aura_helper.CreateEnchantmentFor(target));
					data_.applied_enchantments.insert(std::make_pair(target, std::move(enchant_identifier)));
				});
			}

		private:
			EntitiesManager & mgr_;
			Entity::AuraAuxData & data_;
		};
	}
}