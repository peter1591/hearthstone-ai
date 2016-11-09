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

			template <typename EnchantmentType, typename T,
				typename std::enable_if_t<IsAttachedEnchantmentType<EnchantmentType::tier>::value, nullptr_t> = nullptr >
			typename TieredEnchantments::ContainerType::Identifier Add(T&& enchantment)
			{
				return data_.enchantments.PushBack<EnchantmentType>(std::forward<T>(enchantment));
			}

			template <typename EnchantmentType, typename T,
				typename std::enable_if_t<!IsAttachedEnchantmentType<EnchantmentType::tier>::value, nullptr_t> = nullptr >
			void Add(T&& enchantment)
			{
				return data_.enchantments.PushBack<EnchantmentType>(std::forward<T>(enchantment));
			}

			template <typename EnchantmentType, typename T,
				typename std::enable_if_t<IsAttachedEnchantmentType<EnchantmentType::tier>::value, nullptr_t> = nullptr >
			void Remove(T&& id)
			{
				return data_.enchantments.Remove<EnchantmentType>(std::forward<T>(id));
			}

		private:
			Entity::EnchantmentAuxData & data_;
		};
	}
}