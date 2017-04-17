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
			typedef void(*RegisterEventFunctor)(FlowControl::Manipulate &);

			struct AuraEnchantment {
				ApplyFunctor apply_functor;

				bool Apply(state::State const& state, state::Cards::EnchantableStates & stats) const { apply_functor(stats); return true; }
			};
			struct NormalEnchantment {
				ApplyFunctor apply_functor;
				int valid_until_turn; // -1 if always valid

				bool Apply(state::State const& state, state::Cards::EnchantableStates & stats) const;
			};
			struct EventHookedEnchantment {
				bool Apply(state::State const& state, state::Cards::EnchantableStates & stats) const { apply_functor(stats); return true; }
				void RegisterEvent(FlowControl::Manipulate & manipulate) const { register_functor(manipulate); }

				ApplyFunctor apply_functor;
				RegisterEventFunctor register_functor;
			};
			using EnchantmentType = std::variant<NormalEnchantment, AuraEnchantment, EventHookedEnchantment>;

			typedef Utils::CloneableContainers::RemovableVector<EnchantmentType> ContainerType;
			using IdentifierType = ContainerType::Identifier;

			class NormalEnchantmentUpdateDecider {
			public:
				NormalEnchantmentUpdateDecider() : need_update_after_turn_(-1) {}
				void PushBack(int valid_until_turn);
				void FinishedUpdate(state::State const& state);
				bool NeedUpdate(state::State const& state) const;

			private:
				int need_update_after_turn_;
			};

			template <typename EnchantmentType>
			typename IdentifierType PushBackAuraEnchantment()
			{
				EnchantmentType item;
				need_update_ = true;
				assert(item.apply_functor);
				return enchantments_.PushBack(AuraEnchantment{ item.apply_functor });
			}

			template <typename EnchantmentType>
			void PushBackNormalEnchantment(state::State const& state)
			{
				EnchantmentType item;
				int valid_until_turn = -1;
				if (item.valid_this_turn) valid_until_turn = state.GetTurn();

				need_update_ = true;
				assert(item.apply_functor);
				enchantments_.PushBack(NormalEnchantment{ item.apply_functor, valid_until_turn });
				normal_enchantment_update_decider_.PushBack(valid_until_turn);
			}

			template <typename EnchantmentType>
			typename IdentifierType PushBackEventHookedEnchantment()
			{
				EnchantmentType item;
				need_update_ = true;
				assert(item.apply_functor);
				assert(item.register_functor);
				return enchantments_.PushBack(EventHookedEnchantment{ item.apply_functor, item.register_functor });
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

			void AfterCopied(FlowControl::Manipulate & manipulate)
			{
				need_update_ = true;
				enchantments_.IterateAll([&](IdentifierType id, EnchantmentType& enchantment) -> bool {
					struct OpFunctor {
						OpFunctor(FlowControl::Manipulate & manipulate, ContainerType & enchantments, IdentifierType id)
							: manipulate_(manipulate), enchantments_(enchantments), id_(id)
						{}

						void operator()(NormalEnchantment const& arg) {
							// do nothing
						}
						void operator()(AuraEnchantment const& arg) {
							// All aura enchantments are removed
							enchantments_.Remove(id_);
						}
						void operator()(EventHookedEnchantment const& arg) {
							arg.RegisterEvent(manipulate_);
						}

						FlowControl::Manipulate & manipulate_;
						ContainerType & enchantments_;
						IdentifierType id_;
					};
					
					std::visit(OpFunctor(manipulate, enchantments_, id), enchantment);
					return true;
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

			bool NeedUpdate(state::State const& state) const {
				if (need_update_) return true;
				if (normal_enchantment_update_decider_.NeedUpdate(state)) return true;
				return false;
			}

			void FinishedUpdate(state::State const& state) {
				need_update_ = false;
				normal_enchantment_update_decider_.FinishedUpdate(state);
			}

		private:
			ContainerType enchantments_;
			bool need_update_;

			NormalEnchantmentUpdateDecider normal_enchantment_update_decider_;
		};
	}
}