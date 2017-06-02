#pragma once

#include <functional>
#include <utility>
#include <memory>
#include "bootstrap/variant"
#include "Utils/CloneableContainers/RemovableVector.h"
#include "state/Cards/EnchantableStates.h"
#include "FlowControl/enchantment/detail/UpdateDecider.h"

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
				bool force_update_every_time;

				AuraEnchantment(ApplyFunctor apply_functor, bool force_update_every_time)
					: apply_functor(apply_functor), force_update_every_time(force_update_every_time)
				{}

				bool Apply(ApplyFunctorContext const& context) const {
					apply_functor(context);
					return true;
				}
			};
			struct NormalEnchantment {
				ApplyFunctor apply_functor;
				int valid_until_turn; // -1 if always valid
				bool force_update_every_time;

				NormalEnchantment(ApplyFunctor apply_functor, int valid_until_turn, bool force_update_every_time)
					: apply_functor(apply_functor), valid_until_turn(valid_until_turn), force_update_every_time(force_update_every_time)
				{}

				bool Apply(ApplyFunctorContext const& context) const;
			};
			struct EventHookedEnchantment {
				struct AuxData {
					AuxData() : valid(false) {}
					bool valid;
					state::PlayerIdentifier player;
				};

				typedef void(*RegisterEventFunctor)(FlowControl::Manipulate const&, state::CardRef, IdentifierType, AuxData &);

				bool Apply(ApplyFunctorContext const& context) const {
					apply_functor(context);
					return true;
				}
				void RegisterEvent(FlowControl::Manipulate const& manipulate, state::CardRef card_ref, IdentifierType id, AuxData & in_aux_data) const {
					register_functor(manipulate, card_ref, id, in_aux_data);
				}

				EventHookedEnchantment(ApplyFunctor apply_functor, RegisterEventFunctor register_functor, AuxData const& aux_data)
					: apply_functor(apply_functor), register_functor(register_functor), aux_data(aux_data)
				{}

				ApplyFunctor apply_functor;
				RegisterEventFunctor register_functor;
				AuxData aux_data;
			};
			using EnchantmentType = bootstrap::variant<NormalEnchantment, AuraEnchantment, EventHookedEnchantment>;
			typedef Utils::CloneableContainers::RemovableVector<EnchantmentType> ContainerType;

			template <typename EnchantmentType>
			IdentifierType PushBackAuraEnchantment(EnchantmentType&& item)
			{
				assert(item.apply_functor);

				update_decider_.AddItem();
				assert(item.valid_this_turn == false);

				if (item.force_update_every_time) update_decider_.AddForceUpdateItem();

				return enchantments_.PushBack(AuraEnchantment(item.apply_functor, item.force_update_every_time));
			}

			template <typename EnchantmentType>
			void PushBackNormalEnchantment(state::State const& state, EnchantmentType&& item);

			template <typename EnchantmentType>
			IdentifierType PushBackEventHookedEnchantment(
				FlowControl::Manipulate const& manipulate, state::CardRef card_ref,
				EnchantmentType&& item, enchantment::Enchantments::EventHookedEnchantment::AuxData const& aux_data)
			{
				assert(item.apply_functor);
				assert(item.register_functor);
				IdentifierType id = enchantments_.PushBack(EventHookedEnchantment(item.apply_functor, item.register_functor, aux_data));
				item.register_functor(manipulate, card_ref, id,
					bootstrap::get<EventHookedEnchantment>(*enchantments_.Get(id)).aux_data);

				update_decider_.AddItem();

				return id;
			}

			void Remove(IdentifierType id)
			{
				update_decider_.RemoveItem();

				struct OpFunctor : boost::static_visitor<void> {
					OpFunctor(detail::UpdateDecider & updater_decider)
						: updater_decider_(updater_decider)
					{}

					void operator()(NormalEnchantment & arg) {
						if (arg.force_update_every_time) updater_decider_.RemoveForceUpdateItem();
					}
					void operator()(AuraEnchantment & arg) {
						if (arg.force_update_every_time) updater_decider_.RemoveForceUpdateItem();
					}
					void operator()(EventHookedEnchantment &) {
					}

					detail::UpdateDecider & updater_decider_;
				};

				auto remove_item = enchantments_.Get(id);
				if (remove_item == nullptr) return;
				boost::apply_visitor(OpFunctor(update_decider_), *remove_item);
				return enchantments_.Remove(id);
			}

			void Clear()
			{
				update_decider_.RemoveItem();
				enchantments_.Clear();
			}

			bool Empty() {
				bool empty = true;
				enchantments_.IterateAll([&](...) {
					empty = false;
					return false;
				});
				return empty;
			}

			void AfterCopied(FlowControl::Manipulate const& manipulate, state::CardRef card_ref)
			{
				update_decider_.RemoveItem();
				enchantments_.IterateAll([&](IdentifierType id, EnchantmentType& enchantment) -> bool {
					struct OpFunctor : boost::static_visitor<void> {
						OpFunctor(FlowControl::Manipulate const& manipulate, state::CardRef card_ref, ContainerType & enchantments, IdentifierType id)
							: manipulate_(manipulate), card_ref_(card_ref), enchantments_(enchantments), id_(id)
						{}

						void operator()(NormalEnchantment &) {
							// do nothing
						}
						void operator()(AuraEnchantment &) {
							// All aura enchantments are removed
							enchantments_.Remove(id_);
						}
						void operator()(EventHookedEnchantment & arg) {
							arg.RegisterEvent(manipulate_, card_ref_, id_, arg.aux_data);
						}

						FlowControl::Manipulate const& manipulate_;
						state::CardRef card_ref_;
						ContainerType & enchantments_;
						IdentifierType id_;
					};
					
                                        boost::apply_visitor(OpFunctor(manipulate, card_ref, enchantments_, id), enchantment);
					return true;
				});
			}

			bool Exists(IdentifierType id) const
			{
				return enchantments_.Get(id) != nullptr;
			}

                        struct ApplyAllFunctor : boost::static_visitor<void> {
                              ApplyAllFunctor(state::State const& state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates & stats, ContainerType & enchantments, IdentifierType id)
                                : state_(state), flow_context_(flow_context), card_ref_(card_ref), stats_(stats), enchantments_(enchantments), id_(id) {}

                          template <typename T>
                              void operator()(T&& arg) {
                                if (!arg.Apply(ApplyFunctorContext{ state_, flow_context_, card_ref_, &stats_ })) {
                                  enchantments_.Remove(id_);
                                }
                              }

                         private:
                              state::State const& state_;
                              FlowContext & flow_context_;
                              state::CardRef card_ref_;
                              state::Cards::EnchantableStates & stats_;
                              ContainerType & enchantments_;
                              IdentifierType id_;
                        };
			void ApplyAll(state::State const& state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates & stats)
			{
				enchantments_.IterateAll([&](IdentifierType id, EnchantmentType& enchantment) -> bool {
                                  boost::apply_visitor(ApplyAllFunctor(state, flow_context, card_ref, stats, enchantments_, id), enchantment);
                                  return true;
				});
			}

			bool NeedUpdate(state::State const& state) const {
				if (update_decider_.NeedUpdate(state)) return true;
				return false;
			}

			void FinishedUpdate(state::State const& state) {
				update_decider_.FinishedUpdate(state);
			}

		private:
			ContainerType enchantments_;
			detail::UpdateDecider update_decider_;
		};
	}
}
