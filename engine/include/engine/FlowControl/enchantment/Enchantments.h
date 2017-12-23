#pragma once

#include <functional>
#include <utility>
#include <memory>
#include <variant>
#include "Utils/CloneableContainers/RemovableVector.h"
#include "state/Cards/EnchantableStates.h"
#include "engine/FlowControl/enchantment/detail/UpdateDecider.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace engine {
	namespace FlowControl
	{
		class FlowContext;
		class Manipulate;

		namespace enchantment
		{
			class Enchantments
			{
			public:
				using IdentifierType = Utils::CloneableContainers::RemovableVectorIdentifier;

				Enchantments() : base_enchantments_(nullptr), enchantments_(), update_decider_() {}

				Enchantments(Enchantments const& rhs) :
					base_enchantments_(rhs.base_enchantments_),
					enchantments_(rhs.enchantments_),
					update_decider_(rhs.update_decider_)
				{}

				Enchantments & operator=(Enchantments const& rhs) {
					base_enchantments_ = rhs.base_enchantments_;
					enchantments_ = rhs.enchantments_;
					update_decider_ = rhs.update_decider_;
					return *this;
				}

				void RefCopy(Enchantments const& base) {
					assert(base.base_enchantments_ == nullptr);
					base_enchantments_ = &base.enchantments_;
					update_decider_ = base.update_decider_;
				}

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
						AuxData() : valid(false), player() {}
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
				using EnchantmentType = std::variant<NormalEnchantment, AuraEnchantment, EventHookedEnchantment>;
				typedef Utils::CloneableContainers::RemovableVector<EnchantmentType> ContainerType;

				template <typename EnchantmentType>
				IdentifierType PushBackAuraEnchantment(EnchantmentType&& item)
				{
					assert(item.apply_functor);

					update_decider_.AddItem();
					assert(item.valid_this_turn == false);

					if (item.force_update_every_time) update_decider_.AddForceUpdateItem();

					return GetEnchantmentsForWrite().PushBack(AuraEnchantment(item.apply_functor, item.force_update_every_time));
				}

				template <typename EnchantmentType>
				void PushBackNormalEnchantment(state::State const& state, EnchantmentType&& item);

				template <typename EnchantmentType>
				IdentifierType PushBackEventHookedEnchantment(
					FlowControl::Manipulate const& manipulate, state::CardRef card_ref,
					EnchantmentType&& item, Enchantments::EventHookedEnchantment::AuxData const& aux_data);

				void Remove(IdentifierType id)
				{
					update_decider_.RemoveItem();

					struct OpFunctor {
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

					auto remove_item = GetEnchantmentsForWrite().Get(id);
					if (remove_item == nullptr) return;
					std::visit(OpFunctor(update_decider_), *remove_item);
					return GetEnchantmentsForWrite().Remove(id);
				}

				void Clear()
				{
					update_decider_.RemoveItem();
					GetEnchantmentsForWrite().Clear();
				}

				bool Empty() {
					bool empty = true;
					GetEnchantmentsForRead().IterateAll([&](...) {
						empty = false;
						return false;
					});
					return empty;
				}

				void AfterCopied(FlowControl::Manipulate const& manipulate, state::CardRef card_ref)
				{
					update_decider_.RemoveItem();
					GetEnchantmentsForWrite().IterateAll([&](IdentifierType id, EnchantmentType& enchantment) -> bool {
						struct OpFunctor {
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

						std::visit(OpFunctor(manipulate, card_ref, GetEnchantmentsForWrite(), id), enchantment);
						return true;
					});
				}

				bool Exists(IdentifierType id) const
				{
					return GetEnchantmentsForRead().Get(id) != nullptr;
				}

				void ApplyAll(state::State const& state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::EnchantableStates & stats)
				{
					// try read-only version first. if failed, try mutable one.
					bool success = true;
					GetEnchantmentsForRead().IterateAll([&](IdentifierType id, EnchantmentType const& enchantment) -> bool {
						std::visit([&](auto&& arg) {
							if (!arg.Apply(ApplyFunctorContext{ state, flow_context, card_ref, &stats })) {
								success = false;
							}
						}, enchantment);
						return success; // continue if success
					});

					if (success) return;

					// Some enchantment should be removed. Retry with mutable one.
					GetEnchantmentsForWrite().IterateAll([&](IdentifierType id, EnchantmentType const& enchantment) -> bool {
						std::visit([&](auto&& arg) {
							if (!arg.Apply(ApplyFunctorContext{ state, flow_context, card_ref, &stats })) {
								GetEnchantmentsForWrite().Remove(id);
							}
						}, enchantment);
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
				ContainerType const& GetEnchantmentsForRead() const {
					if (base_enchantments_) return *base_enchantments_;
					else return enchantments_;
				}

				ContainerType & GetEnchantmentsForWrite() {
					if (base_enchantments_) {
						enchantments_ = *base_enchantments_; // copy on write
						base_enchantments_ = nullptr;
					}
					return enchantments_;
				}

			private:
				ContainerType const* base_enchantments_;
				ContainerType enchantments_;
				detail::UpdateDecider update_decider_;
			};
		}
	}
}