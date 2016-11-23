#pragma once

#include "Entity/AuraAuxData.h"
#include "State/State.h"

namespace Manipulators
{
	namespace Helpers
	{
		class AuraHelper
		{
		public:
			AuraHelper(State::State & state, Entity::Card & card) :
				state_(state),
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
						StateManipulator(state_).Minion(it->first).Enchant().Remove<EnchantmentType>(it->second);
						it = data_.applied_enchantments.erase(it);
					}
				}

				client_aura_helper.IterateEligibles([this, &client_aura_helper](CardRef target) {
					if (data_.applied_enchantments.find(target) != data_.applied_enchantments.end()) return; // already applied

					auto enchant_identifier = StateManipulator(state_).Minion(target).Enchant()
						.Add(client_aura_helper.CreateEnchantmentFor(target));
					data_.applied_enchantments.insert(std::make_pair(target, std::move(enchant_identifier)));
				});
			}

		private:
			State::State & state_;
			Entity::AuraAuxData & data_;
		};
	}
}