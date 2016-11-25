#pragma once

#include <type_traits>
#include <utility>
#include "State/Events/impl/HandlersContainer.h"
#include "State/Events/impl/CategorizedHandlersContainer.h"
#include "State/Events/TriggerTypes/AfterMinionSummoned.h"
#include "State/Events/TriggerTypes/BeforeMinionSummoned.h"
#include "State/Events/TriggerTypes/AfterMinionPlayed.h"
#include "State/Events/TriggerTypes/OnMinionPlay.h"

namespace state
{
	namespace Events
	{
		template <typename T> class Event;
		template <typename T> class CategorizedEvent;

		class HandlersManager
		{
			template <typename T> friend class Event;
			template <typename T> friend class CategorizedEvent;

		public:
			template <typename T>
			void PushBack(T&& handler) {
				GetHandlersContainer<std::decay_t<T>>().PushBack(std::forward<T>(handler));
			}

			template <typename T1, typename T2>
			void PushBack(T1&& category, T2&& handler) {
				GetHandlersContainer<std::decay_t<T1>, std::decay_t<T2>>().PushBack(
					std::forward<T1>(category), std::forward<T2>(handler));
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerEvent(Args&&... args)
			{
				return GetHandlersContainer<EventTriggerType>().TriggerAll(std::forward<Args>(args)...);
			}

			template <typename EventTriggerType, typename CategoryType, typename... Args>
			void TriggerCategorizedEvent(CategoryType&& category, Args&&... args)
			{
				return GetHandlersContainer<std::decay_t<CategoryType>, EventTriggerType>()
					.TriggerAll(std::forward<CategoryType>(category), std::forward<Args>(args)...);
			}

		private:
			template<typename EventHandlerType>
			impl::HandlersContainer<EventHandlerType> & GetHandlersContainer();

			template<typename Category, typename EventHandlerType>
			impl::CategorizedHandlersContainer<Category, EventHandlerType> & GetHandlersContainer();

#define ADD_TRIGGER_TYPE_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::HandlersContainer<TriggerTypes::TYPE_NAME> MEMBER_NAME; \
private: \
	template <> impl::HandlersContainer<TriggerTypes::TYPE_NAME> & GetHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_TRIGGER_TYPE(TYPE_NAME) ADD_TRIGGER_TYPE_INTERNAL(TYPE_NAME, handler_ ## TYPE_NAME ## _)

#define ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::CategorizedHandlersContainer<int, TriggerTypes::TYPE_NAME> MEMBER_NAME; \
private: \
	template <> impl::CategorizedHandlersContainer<int, TriggerTypes::TYPE_NAME> & GetHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_CATEGORIZED_TRIGGER_TYPE(TYPE_NAME) ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL(TYPE_NAME, categorized_handler_ ## TYPE_NAME ## _)

			ADD_TRIGGER_TYPE(AfterMinionSummoned);
			ADD_TRIGGER_TYPE(BeforeMinionSummoned);
			ADD_TRIGGER_TYPE(AfterMinionPlayed);
			ADD_TRIGGER_TYPE(OnMinionPlay);

			ADD_CATEGORIZED_TRIGGER_TYPE(AfterMinionSummoned);
			ADD_CATEGORIZED_TRIGGER_TYPE(BeforeMinionSummoned);
			ADD_CATEGORIZED_TRIGGER_TYPE(AfterMinionPlayed);
			ADD_CATEGORIZED_TRIGGER_TYPE(OnMinionPlay);

#undef ADD_TRIGGER_TYPE_INTERNAL
#undef ADD_TRIGGER_TYPE
#undef ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL
#undef ADD_CATEGORIZED_TRIGGER_TYPE
		};

	}
}