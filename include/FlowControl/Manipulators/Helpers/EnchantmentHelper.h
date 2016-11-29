#pragma once

#include <memory>
#include <utility>
#include "State/Cards/Card.h"
#include "State/Cards/EnchantmentAuxData.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace Helpers
		{
			class EnchantmentHelper
			{
			public:
				EnchantmentHelper(state::Cards::Card &card) :
					data_(card.GetMutableEnchantmentAuxDataGetter().Get())
				{
				}

				template <typename T>
				decltype(auto) Add(T&& enchantment)
				{
					data_.need_update = true;
					return data_.enchantments.PushBack(std::forward<T>(enchantment));
				}

				template <typename T>
				decltype(auto) AddAuraEnchant(T&& enchantment)
				{
					data_.need_update = true;
					return data_.enchantments.PushBackAuraEnchant(std::forward<T>(enchantment));
				}

				template <typename EnchantmentType, typename T>
				decltype(auto) Remove(T&& id)
				{
					data_.need_update = true;
					return data_.enchantments.Remove<EnchantmentType>(std::forward<T>(id));
				}

				template <typename T>
				decltype(auto) RemoveAuraEnchantment(T&& id)
				{
					data_.need_update = true;
					return data_.enchantments.RemoveAuraEnchant(std::forward<T>(id));
				}

			private:
				state::Cards::EnchantmentAuxData & data_;
			};
		}
	}
}