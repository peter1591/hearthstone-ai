#pragma once

#include <utility>
#include "EventManager/impl/HandlersContainer.h"
#include "EventManager/impl/CategorizedHandlersContainer.h"
#include "EventManager/Handlers/MinionSummoned.h"
#include "EventManager/Handlers/MinionSummonedOnce.h"

namespace EventManager
{
	class Trigger
	{
	public:
		// Cloneable by copy semantics
		//    Since the underlying data structures are all with this property.
		static const bool CloneableByCopySemantics = true;

		template <typename EventHandlerType_>
		void PushBack(EventHandlerType_&& handler) {
			using EventHandlerType = typename std::remove_reference<EventHandlerType_>::type;
			GetHandlersContainer<EventHandlerType>().PushBack(std::forward<EventHandlerType_>(handler));
		}

		template <typename CategoryType_, typename EventHandlerType_>
		void PushBack(CategoryType_&& category, EventHandlerType_&& handler) {
			using CategoryType = typename std::remove_reference<CategoryType_>::type;
			using EventHandlerType = typename std::remove_reference<EventHandlerType_>::type;

			GetHandlersContainer<CategoryType, EventHandlerType>().PushBack(
				std::forward<CategoryType_>(category), std::forward<EventHandlerType_>(handler));
		}

	public:
		template<typename EventHandlerType>
		impl::HandlersContainer<EventHandlerType> & GetHandlersContainer();

		template<typename Category, typename EventHandlerType>
		impl::CategorizedHandlersContainer<Category, EventHandlerType> & GetHandlersContainer();

#define ADD_HANDLER_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::HandlersContainer<Handlers::TYPE_NAME> MEMBER_NAME; \
public: \
	template <> impl::HandlersContainer<Handlers::TYPE_NAME> & GetHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_HANDLER(TYPE_NAME) ADD_HANDLER_INTERNAL(TYPE_NAME, handler_ ## TYPE_NAME ## _)

#define ADD_CATEGORIZED_HANDLER_INTERNAL(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::CategorizedHandlersContainer<int, Handlers::TYPE_NAME> MEMBER_NAME; \
public: \
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