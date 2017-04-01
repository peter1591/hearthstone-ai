#pragma once

#include <functional>
#include <utility>
#include <memory>
#include <variant>
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
			typedef void(*ApplyFunctor)(state::Cards::EnchantableStates &);

			struct AuraEnchantment {
				ApplyFunctor apply_functor;

				bool Apply(state::State const& state, state::Cards::EnchantableStates & stats) { apply_functor(stats); return true; }
			};
			struct NormalEnchantment {
				ApplyFunctor apply_functor;
				int valid_until_turn; // -1 if always valid

				bool Apply(state::State const& state, state::Cards::EnchantableStates & stats);
			};
			using EnchantmentType = std::variant<NormalEnchantment, AuraEnchantment>;

			typedef Utils::CloneableContainers::RemovableVector<EnchantmentType> ContainerType;
			using IdentifierType = ContainerType::Identifier;

			template <typename EnchantmentType>
			typename IdentifierType PushBackAuraEnchantment()
			{
				EnchantmentType item;
				need_update_ = true;
				return enchantments_.PushBack(AuraEnchantment{ item.apply_functor });
			}

			template <typename EnchantmentType>
			void PushBackNormalEnchantment(state::State const& state)
			{
				EnchantmentType item;
				int valid_until_turn = -1;
				if (item.valid_this_turn) valid_until_turn = state.GetTurn();

				need_update_ = true;
				enchantments_.PushBack(NormalEnchantment{ item.apply_functor, valid_until_turn });
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

			void AfterCopied()
			{
				// All aura enchantments are removed
				need_update_ = true;
				enchantments_.IterateAll([this](IdentifierType id, EnchantmentType& enchantment) -> bool {
					return std::visit([&](auto&& arg) -> bool {
						using T = std::decay_t<decltype(arg)>;
						if (std::is_same_v<T, AuraEnchantment>) {
							enchantments_.Remove(id);
						}
						return true;
					}, enchantment);
				});
			}

			bool Exists(IdentifierType id) const
			{
				return enchantments_.Get(id) != nullptr;
			}

			void ApplyAll(state::State const& state, state::Cards::EnchantableStates & stats)
			{
				enchantments_.IterateAll([&](IdentifierType id, EnchantmentType& enchantment) -> bool {
					std::visit([&](auto&& arg) {
						if (!arg.Apply(state, stats)) {
							enchantments_.Remove(id);
						}
					}, enchantment);
					return true;
				});
			}

			bool NeedUpdate() const {
				return true; // TODO: special tricks for one-turn enchantments
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