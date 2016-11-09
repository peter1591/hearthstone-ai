#pragma once

#include <utility>
#include "Entity/EnchantmentAuxData.h"

namespace Manipulators
{
	namespace Helpers
	{
		class EnchantmentHelper
		{
		public:
			EnchantmentHelper(Entity::EnchantmentAuxData & data) : data_(data) {}

			template <typename EnchantmentType, typename T>
			decltype(auto) Add(T&& enchantment)
			{
				return data_.enchantments.PushBack<EnchantmentType>(std::forward<T>(enchantment));
			}

			template <typename EnchantmentType, typename T>
			void Remove(T&& id)
			{
				return data_.enchantments.Remove<EnchantmentType>(std::forward<T>(id));
			}

		private:
			Entity::EnchantmentAuxData & data_;
		};
	}
}