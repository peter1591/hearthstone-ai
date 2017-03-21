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
		class Enchantments
		{
		public:
			Enchantments() : need_update_(false), valid_until_turn_count_(0) {}

			typedef void(*ApplyFunctor)(state::Cards::EnchantableStates &);
			struct ManagedEnchantment {
				ApplyFunctor apply_functor;
				int valid_until_turn; // -1 if always valid
			};
			typedef Utils::CloneableContainers::RemovableVector<ManagedEnchantment> ContainerType;

			typename ContainerType::Identifier PushBack(ApplyFunctor apply_functor, int valid_until_turn)
			{
				need_update_ = true;
				if (valid_until_turn > 0) ++valid_until_turn_count_;
				return enchantments_.PushBack(ManagedEnchantment{apply_functor, valid_until_turn});
			}

			void Remove(ContainerType::Identifier id)
			{
				need_update_ = true;
				if (enchantments_.Get(id)->valid_until_turn > 0) --valid_until_turn_count_;
				return enchantments_.Remove(id);
			}

			void Clear()
			{
				need_update_ = true;
				valid_until_turn_count_ = 0;
				enchantments_.Clear();
			}

			bool Exists(ContainerType::Identifier id) const
			{
				return enchantments_.Get(id) != nullptr;
			}

			void ApplyAll(state::Cards::EnchantableStates & card, state::State const& state);

			bool NeedUpdate() const {
				if (valid_until_turn_count_ > 0) return true;
				return need_update_;
			}

			void FinishedUpdate() {
				need_update_ = false;
			}

		private:
			ContainerType enchantments_;
			bool need_update_;
			int valid_until_turn_count_;
		};
	}
}