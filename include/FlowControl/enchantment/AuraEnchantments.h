#pragma once

#include <functional>
#include <utility>
#include <memory>
#include "Utils/CloneableContainers/RemovableVector.h"
#include "state/Cards/EnchantableStates.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace FlowControl
{
	namespace enchantment
	{
		class AuraEnchantments
		{
		public:
			typedef void(*ApplyFunctor)(state::Cards::EnchantableStates &);
			typedef Utils::CloneableContainers::RemovableVector<ApplyFunctor> ContainerType;

			template <typename T>
			typename ContainerType::Identifier PushBack(T && item)
			{
				return enchantments_.PushBack(item);
			}

			void Remove(ContainerType::Identifier id)
			{
				return enchantments_.Remove(id);
			}

			void Clear()
			{
				enchantments_.Clear();
			}

			bool Exists(ContainerType::Identifier id) const
			{
				return enchantments_.Get(id) != nullptr;
			}

			void ApplyAll(state::Cards::EnchantableStates & card)
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