#pragma once

#include <type_traits>
#include <utility>
#include "EventManager/impl/HandlersContainer.h"
#include "EventManager/impl/CategorizedHandlersContainer.h"
#include "EventManager/TriggerTypes/MinionSummoned.h"

namespace EventManager
{
	template <typename T> class Event;
	template <typename T> class CategorizedEvent;
	template <typename T> class StaticEvent;
	template <typename T> class StaticCategorizedEvent;

	class HandlersManager
	{
		template <typename T> friend class Event;
		template <typename T> friend class CategorizedEvent;
		template <typename T> friend class StaticEvent;
		template <typename T> friend class StaticCategorizedEvent;

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

		ADD_TRIGGER_TYPE(MinionSummoned);
		ADD_CATEGORIZED_TRIGGER_TYPE(MinionSummoned);

#undef ADD_TRIGGER_TYPE_INTERNAL
#undef ADD_TRIGGER_TYPE
#undef ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL
#undef ADD_CATEGORIZED_TRIGGER_TYPE
	};

}