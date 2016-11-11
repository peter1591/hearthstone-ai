#pragma once

#include <memory>
#include <utility>
#include "Entity/Card.h"
#include "Enchantment/Base.h"
#include "Entity/EnchantmentAuxData.h"

namespace Manipulators
{
	namespace Helpers
	{
		class EnchantmentHelper
		{
		public:
			EnchantmentHelper(Entity::Card &card) :
				data_(card.GetMutableEnchantmentAuxDataGetter().Get())
			{
			}

			template <typename EnchantmentType, typename... Args>
			decltype(auto) CreateAndAdd(Args&&... args)
			{
				auto instance = std::unique_ptr<Enchantment::Base>(new EnchantmentType(std::forward<Args>(args)...));
				return this->Add<EnchantmentType>(std::move(instance));
			}

			template <typename EnchantmentType, typename T>
			decltype(auto) Add(T&& enchantment)
			{
				data_.need_update = true;
				return data_.enchantments.PushBack<EnchantmentType>(std::forward<T>(enchantment));
			}

			template <typename EnchantmentType, typename T>
			decltype(auto) Remove(T&& id)
			{
				data_.need_update = true;
				return data_.enchantments.Remove<EnchantmentType>(std::forward<T>(id));
			}

		private:
			Entity::EnchantmentAuxData & data_;
		};
	}
}