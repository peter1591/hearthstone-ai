#pragma once

#include <memory>
#include <utility>
#include "State/Cards/Card.h"
#include "State/Cards/EnchantmentAuxData.h"

namespace Manipulators
{
	namespace Helpers
	{
		class EnchantmentHelper
		{
		public:
			EnchantmentHelper(State::Cards::Card &card) :
				data_(card.GetMutableEnchantmentAuxDataGetter().Get())
			{
			}

			template <typename T>
			decltype(auto) Add(T&& enchantment)
			{
				data_.need_update = true;
				return data_.enchantments.PushBack(std::forward<T>(enchantment));
			}

			template <typename EnchantmentType, typename T>
			decltype(auto) Remove(T&& id)
			{
				data_.need_update = true;
				return data_.enchantments.Remove<EnchantmentType>(std::forward<T>(id));
			}

		private:
			State::Cards::EnchantmentAuxData & data_;
		};
	}
}