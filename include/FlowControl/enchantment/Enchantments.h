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
	class FlowContext;
}

namespace FlowControl
{
	namespace enchantment
	{
		class Enchantments
		{
		public:
			using IdentifierType = Utils::CloneableContainers::RemovableVectorIdentifier;

			struct ApplyFunctorContext {
				state::State const& state_;
				FlowContext & flow_context_;
				state::CardRef card_ref_;
				state::Cards::EnchantableStates * stats_;
			};
			typedef void(*ApplyFunctor)(ApplyFunctorContext const&);

			struct AuraEnchantment {
				ApplyFunctor apply_functor;

				bool Apply(ApplyFunctorContext const& context) const {
					apply_functor(context);
					return true;
				}
			};
			struct NormalEnchantment {
				ApplyFunctor apply_functor;
				int valid_until_turn; // -1 if always valid

				bool Apply(ApplyFunctorContext const& context) const;
			};
			struct EventHookedEnchantment {
				struct AuxData {
					AuxData() : valid(false) {}
					bool valid;
					state::PlayerIdentifier player;
				};

				typedef void(*RegisterEventFunctor)(FlowControl::Manipulate &, state::CardRef, IdentifierType, AuxData &);

				bool Apply(ApplyFunctorContext const& context) const {
					apply_functor(context);
					return true;
				}
				void RegisterEvent(FlowControl::Manipulate & manipulate, state::CardRef card_ref, IdentifierType id, AuxData & aux_data) const {
					register_functor(manipulate, card_ref, id, aux_data);
				}

				ApplyFunctor apply_functor;
				RegisterEventFunctor register_functor;
				AuxData aux_data;
			};
			using EnchantmentType = std::variant<NormalEnchantment, AuraEnchantment, EventHookedEnchantment>;
			typedef Utils::CloneableContainers::RemovableVector<EnchantmentType> ContainerType;

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
			typename IdentifierType PushBackAuraEnchantment(EnchantmentType&& item)
			{
				need_update_ = true;
				assert(item.apply_functor);
				return enchantments_.PushBack(AuraEnchantment{ item.apply_functor });
			}

			template <typename EnchantmentType>
			void PushBackNormalEnchantment(state::State const& state, EnchantmentType&& item)
			{
				int valid_until_turn = -1;
				if (item.valid_this_turn) valid_until_turn = state.GetTurn();

				need_update_ = true;
				assert(item.apply_functor);
				enchantments_.PushBack(NormalEnchantment{ item.apply_functor, valid_until_turn });
				normal_enchantment_update_decider_.PushBack(valid_until_turn);
			}

			template <typename EnchantmentType>
			typename IdentifierType PushBackEventHookedEnchantment(
				FlowControl::Manipulate & manipulate, state::CardRef card_ref,
				EnchantmentType&& item, enchantment::Enchantments::EventHookedEnchantment::AuxData const& aux_data)
			{
				need_update_ = true;
				assert(item.apply_functor);
				assert(item.register_functor);
				IdentifierType id = enchantments_.PushBack(EventHookedEnchantment{ item.apply_functor, item.register_functor, aux_data });
				item.register_functor(manipulate, card_ref, id,
					std::get<EventHookedEnchantment>(*enchantments_.Get(id)).aux_data);
				return id;
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

			void AfterCopied(FlowControl::Manipulate & manipulate, state::CardRef card_ref)
			{
				need_update_ = true;
				enchantments_.IterateAll([&](IdentifierType id, EnchantmentType& enchantment) -> bool {
					struct OpFunctor {
						OpFunctor(FlowControl::Manipulate & manipulate, state::CardRef card_ref, ContainerType & enchantments, IdentifierType id)
							: manipulate_(manipulate), card_ref_(card_ref), enchantments_(enchantments), id_(id)
						{}

						void operator()(NormalEnchantment & arg) {
							// do nothing
						}
						void operator()(AuraEnchantment & arg) {
							// All aura enchantments are removed
							enchantments_.Remove(id_);
						}
						void operator()(EventHookedEnchantment & arg) {
							arg.RegisterEvent(manipulate_, card_ref_, id_, arg.aux_data);
						}

						FlowControl::Manipulate & manipulate_;
						state::CardRef card_ref_;
						ContainerType & enchantments_;
						IdentifierType id_;
					};
					
					std::visit(OpFunctor(manipulate, card_ref, enchantments_, id), enchantment);
					return true;
				});
			}

			bool Exists(IdentifierType id) const
			{
				return enchantments_.Get(id) != nullptr;
			}

			void ApplyAll(state::State const& state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates & stats)
			{
				enchantments_.IterateAll([&](IdentifierType id, EnchantmentType& enchantment) -> bool {
					std::visit([&](auto&& arg) {
						if (!arg.Apply(ApplyFunctorContext{ state, flow_context, card_ref, &stats })) {
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