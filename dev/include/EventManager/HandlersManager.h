#pragma once

#include <type_traits>
#include <utility>
#include "EventManager/impl/HandlersContainer.h"
#include "EventManager/impl/CategorizedHandlersContainer.h"
#include "EventManager/Handlers/MinionSummoned.h"
#include "EventManager/Handlers/MinionSummonedOnce.h"

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
		// Cloneable by copy semantics
		//    Since the underlying data structures are all with this property.
		static const bool CloneableByCopySemantics = true;

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

#define ADD_HANDLER_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::HandlersContainer<Handlers::TYPE_NAME> MEMBER_NAME; \
private: \
	template <> impl::HandlersContainer<Handlers::TYPE_NAME> & GetHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_HANDLER(TYPE_NAME) ADD_HANDLER_INTERNAL(TYPE_NAME, handler_ ## TYPE_NAME ## _)

#define ADD_CATEGORIZED_HANDLER_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::CategorizedHandlersContainer<int, Handlers::TYPE_NAME> MEMBER_NAME; \
private: \
	template <> impl::CategorizedHandlersContainer<int, Handlers::TYPE_NAME> & GetHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_CATEGORIZED_HANDLER(TYPE_NAME) ADD_CATEGORIZED_HANDLER_INTERNAL(TYPE_NAME, categorized_handler_ ## TYPE_NAME ## _)

		ADD_HANDLER(MinionSummoned);
		ADD_HANDLER(MinionSummonedOnce);
		ADD_CATEGORIZED_HANDLER(MinionSummoned);
		ADD_CATEGORIZED_HANDLER(MinionSummonedOnce);

#undef ADD_HANDLER_INTERNAL
#undef ADD_HANDLER
#undef ADD_CATEGORIZED_HANDLER_INTERNAL
#undef ADD_CATEGORIZED_HANDLER
	};

}