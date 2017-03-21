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
			using IdentifierType = ContainerType::Identifier;

			template <typename EnchantmentType>
			typename IdentifierType PushBack()
			{
				EnchantmentType item;
				need_update_ = true;
				return enchantments_.PushBack(item.apply_functor);
			}

			void Remove(IdentifierType id)
			{
				need_update_ = true;
				return enchantments_.Remove(id);
			}

			void Clear()
			{
				need_update_ = true;
				enchantments_.Clear();
			}

			bool Exists(IdentifierType id) const
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

			bool NeedUpdate() const {
				return need_update_;
			}

			void FinishedUpdate() {
				need_update_ = false;
			}

		private:
			ContainerType enchantments_;
			bool need_update_;
		};
	}
}