#pragma once

#include <functional>
#include <utility>
#include <memory>
#include "Utils/CloneableContainers/RemovableVector.h"

namespace FlowControl
{
	namespace Context
	{
		class EnchantmentAfterAdded;
	}
}

namespace state
{
	class State;
	namespace Cards
	{
		class Card;

		class Enchantments
		{
		public:
			typedef std::function<void(Card &)> ApplyFunctor;
			typedef Utils::CloneableContainers::RemovableVector<ApplyFunctor> ContainerType;

			typedef void AfterAddedCallback(FlowControl::Context::EnchantmentAfterAdded &);

			template <typename T>
			typename ContainerType::Identifier PushBack(T && item)
			{
				return enchantments_.PushBack(item);
			}

			template <typename T>
			void Remove(T&& id)
			{
				return enchantments_.Remove(std::forward<T>(id));
			}

			template <typename T>
			bool Exists(T&& id) const
			{
				return enchantments_.Get(std::forward<T>(id)) != nullptr;
			}

			void ApplyAll(Card & card)
			{
				enchantments_.IterateAll([&card](ApplyFunctor& functor) -> bool {
					functor(card);
					return true;
				});
			}

		private:
			ContainerType enchantments_;
		};
	}
}