#pragma once

#include <memory>
#include <utility>
#include "Enchantment/Base.h"
#include "Entity/EnchantmentAuxData.h"

namespace Manipulators
{
	namespace Helpers
	{
		class EnchantmentHelper
		{
		public:
			EnchantmentHelper(Entity::EnchantmentAuxData & data) : data_(data) {}

			template <typename EnchantmentType, typename... Args>
			decltype(auto) CreateAndAdd(Args&&... args)
			{
				auto instance = std::unique_ptr<Enchantment::Base>(new EnchantmentType(std::forward<Args>(args)...));
				return this->Add<EnchantmentType>(std::move(instance));
			}

			template <typename EnchantmentType, typename T>
			decltype(auto) Add(T&& enchantment)
			{
				return data_.enchantments.PushBack<EnchantmentType>(std::forward<T>(enchantment));
			}

			template <typename EnchantmentType, typename T>
			decltype(auto) Remove(T&& id)
			{
				return data_.enchantments.Remove<EnchantmentType>(std::forward<T>(id));
			}

		private:
			Entity::EnchantmentAuxData & data_;
		};
	}
}