#pragma once

#include <type_traits>
#include <utility>
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

		public:
			Manager() : event_trigger_recursive_count_(0) {}

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
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) throw std::runtime_error("reach maximum event trigger recursive");
				++event_trigger_recursive_count_;
				GetHandlersContainer<EventTriggerType>().TriggerAll(std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerCategorizedEvent(CardRef card_ref, Args&&... args)
			{
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) throw std::runtime_error("reach maximum event trigger recursive");
				++event_trigger_recursive_count_;
				GetCategorizedHandlersContainer<EventTriggerType>()
					.TriggerAll(card_ref, std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

		private:
			constexpr static int max_event_trigger_recursive_ = 100;
			int event_trigger_recursive_count_;

		private:
			template<typename EventHandlerType>
			impl::HandlersContainer<EventHandlerType> & GetHandlersContainer();

			template<typename EventHandlerType>
			impl::CategorizedHandlersContainer<EventHandlerType> & GetCategorizedHandlersContainer();

#define ADD_TRIGGER_TYPE_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::HandlersContainer<EventTypes::TYPE_NAME> MEMBER_NAME; \
private: \
	template <> impl::HandlersContainer<EventTypes::TYPE_NAME> & GetHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_TRIGGER_TYPE(TYPE_NAME) ADD_TRIGGER_TYPE_INTERNAL(TYPE_NAME, handler_ ## TYPE_NAME ## _)

#define ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::CategorizedHandlersContainer<EventTypes::TYPE_NAME> MEMBER_NAME; \
private: \
	template <> impl::CategorizedHandlersContainer<EventTypes::TYPE_NAME> & GetCategorizedHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_CATEGORIZED_TRIGGER_TYPE(TYPE_NAME) ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL(TYPE_NAME, categorized_handler_ ## TYPE_NAME ## _)

			ADD_TRIGGER_TYPE(GetPlayCardCost);

			ADD_TRIGGER_TYPE(AfterMinionSummoned);
			ADD_TRIGGER_TYPE(BeforeMinionSummoned);
			ADD_TRIGGER_TYPE(AfterMinionPlayed);
			ADD_TRIGGER_TYPE(AfterMinionDied);

			ADD_TRIGGER_TYPE(PreparePlayCardTarget)
			ADD_TRIGGER_TYPE(OnPlay);
			ADD_TRIGGER_TYPE(CheckPlayCardCountered)

			ADD_TRIGGER_TYPE(OnTurnEnd);
			ADD_TRIGGER_TYPE(OnTurnStart);

			ADD_TRIGGER_TYPE(BeforeSecretReveal);

			ADD_TRIGGER_TYPE(PrepareAttackTarget);
			ADD_TRIGGER_TYPE(BeforeAttack);
			ADD_TRIGGER_TYPE(AfterAttack);

			ADD_TRIGGER_TYPE(CalculateHealDamageAmount);
			ADD_TRIGGER_TYPE(PrepareHealDamageTarget);
			ADD_TRIGGER_TYPE(OnTakeDamage);
			ADD_CATEGORIZED_TRIGGER_TYPE(CategorizedOnTakeDamage);
			ADD_TRIGGER_TYPE(OnHeal);
			ADD_TRIGGER_TYPE(AfterTakenDamage)

			ADD_TRIGGER_TYPE(AfterHeroPower);
			ADD_TRIGGER_TYPE(AfterSpellPlayed);
			ADD_TRIGGER_TYPE(AfterSecretPlayed);

#undef ADD_TRIGGER_TYPE_INTERNAL
#undef ADD_TRIGGER_TYPE
#undef ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL
#undef ADD_CATEGORIZED_TRIGGER_TYPE
		};

	}
}