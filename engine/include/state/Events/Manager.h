#pragma once

#include <type_traits>
#include <utility>
#include <tuple>

#include "state/Types.h"
#include "state/Events/impl/HandlersContainer.h"
#include "state/Events/impl/CategorizedHandlersContainer.h"
#include "state/Events/EventTypes.h"

namespace state
{
	namespace Events
	{
		template <typename T> class Event;
		template <typename T> class CategorizedEvent;

		class Manager
		{
			template <typename T> friend class Event;
			template <typename T> friend class CategorizedEvent;

		private:
			struct CopyOnWriteHelper {
				template <int N>
				struct ForEachUnroll {
					template <class Tuple>
					static void Do(Tuple & tuple, Tuple const& base) {
						if constexpr (N >= 0) {
							std::get<N>(tuple).RefCopy(std::get<N>(base));
							ForEachUnroll<N - 1>::Do(tuple, base);
						}
					}
				};
				
				template <typename Tuple>
				static void CopyOnWrite(Tuple & tuple, Tuple const& base) {
					constexpr int tuple_size = std::tuple_size<Tuple>::value;
					ForEachUnroll<tuple_size-1>::Do(tuple, base);
				}
			};

		public:
			Manager() : event_trigger_recursive_count_(0), event_tuple_(), categorized_event_tuple_() {}

			void RefCopy(Manager const& base) {
				event_trigger_recursive_count_ = base.event_trigger_recursive_count_;
				CopyOnWriteHelper::CopyOnWrite(this->event_tuple_, base.event_tuple_);
				CopyOnWriteHelper::CopyOnWrite(this->categorized_event_tuple_, base.categorized_event_tuple_);
			}

			template <typename EventType, typename T>
			void PushBack(T&& handler) {
				GetHandlersContainer<EventType>().PushBack(std::forward<T>(handler));
			}

			template <typename EventType, typename T>
			void PushBack(CardRef card_ref, T&& handler) {
				GetCategorizedHandlersContainer<EventType>().PushBack(
					card_ref, std::forward<T>(handler));
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerEvent(Args&&... args)
			{
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) return;
				++event_trigger_recursive_count_;
				GetHandlersContainer<EventTriggerType>().TriggerAll(std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerCategorizedEvent(CardRef card_ref, Args&&... args)
			{
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) return;
				++event_trigger_recursive_count_;
				GetCategorizedHandlersContainer<EventTriggerType>()
					.TriggerAll(card_ref, std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerEventWithoutRemove(Args&&... args) const
			{
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) return;
				++event_trigger_recursive_count_;
				GetHandlersContainer<EventTriggerType>().TriggerAllWithoutRemove(std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerCategorizedEventWithoutRemove(CardRef card_ref, Args&&... args) const
			{
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) return;
				++event_trigger_recursive_count_;
				GetCategorizedHandlersContainer<EventTriggerType>()
					.TriggerAllWithoutRemove(card_ref, std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

		private:
			constexpr static int max_event_trigger_recursive_ = 100;
			mutable int event_trigger_recursive_count_; // obey logical const meanings

		private:
			using EvnetTypesTuple = std::tuple<
				impl::HandlersContainer<EventTypes::GetPlayCardCost>,
				impl::HandlersContainer<EventTypes::AfterMinionSummoned>,
				impl::HandlersContainer<EventTypes::BeforeMinionSummoned>,
				impl::HandlersContainer<EventTypes::AfterMinionPlayed>,
				impl::HandlersContainer<EventTypes::AfterMinionDied>,
				impl::HandlersContainer<EventTypes::PreparePlayCardTarget>,
				impl::HandlersContainer<EventTypes::OnPlay>,
				impl::HandlersContainer<EventTypes::CheckPlayCardCountered>,
				impl::HandlersContainer<EventTypes::OnTurnEnd>,
				impl::HandlersContainer<EventTypes::OnTurnStart>,
				impl::HandlersContainer<EventTypes::BeforeSecretReveal>,
				impl::HandlersContainer<EventTypes::PrepareAttackTarget>,
				impl::HandlersContainer<EventTypes::BeforeAttack>,
				impl::HandlersContainer<EventTypes::AfterAttack>,
				impl::HandlersContainer<EventTypes::CalculateHealDamageAmount>,
				impl::HandlersContainer<EventTypes::PrepareHealDamageTarget>,
				impl::HandlersContainer<EventTypes::OnTakeDamage>,
				impl::HandlersContainer<EventTypes::OnHeal>,
				impl::HandlersContainer<EventTypes::AfterTakenDamage>,
				impl::HandlersContainer<EventTypes::AfterHeroPower>,
				impl::HandlersContainer<EventTypes::AfterSpellPlayed>,
				impl::HandlersContainer<EventTypes::AfterSecretPlayed>
			>;
			EvnetTypesTuple event_tuple_;

			using CategorizedEventTypesTuple = std::tuple<
				impl::CategorizedHandlersContainer<EventTypes::CategorizedOnTakeDamage>
			>;
			CategorizedEventTypesTuple categorized_event_tuple_;

			template <typename EventType>
			impl::HandlersContainer<EventType> & GetHandlersContainer() {
				return std::get<impl::HandlersContainer<EventType>>(event_tuple_);
			}
			template <typename EventType>
			impl::HandlersContainer<EventType> const& GetHandlersContainer() const {
				return std::get<impl::HandlersContainer<EventType>>(event_tuple_);
			}

			template<typename EventType>
			impl::CategorizedHandlersContainer<EventType> & GetCategorizedHandlersContainer() {
				return std::get<impl::CategorizedHandlersContainer<EventType>>(categorized_event_tuple_);
			}
			template<typename EventType>
			impl::CategorizedHandlersContainer<EventType> const& GetCategorizedHandlersContainer() const {
				return std::get<impl::CategorizedHandlersContainer<EventType>>(categorized_event_tuple_);
			}
		};
	}
}
