#pragma once

#include "EventManager/HandlersContainer.h"
#include "EventManager/Handlers/MinionSummoned.h"

namespace EventManager
{
	class Manager
	{
	public:
		template<typename EventHandlerType>
		typename HandlersContainer<EventHandlerType>::Token
		PushBack(const EventHandlerType& handler) {
			return GetHandlersContainer<EventHandlerType>().PushBack(handler);
		}

		template <typename Token>
		void Remove(Token token) {
			GetHandlersContainer<typename Token::HandlersContainerType::HandlerType>().Remove(token);
		}

		template <typename EventHandlerType, typename... Args>
		void TriggerEvent(const Args&... args) {
			GetHandlersContainer<EventHandlerType>().TriggerAll(args...);
		}

	private:
		template<typename EventHandlerType>
		HandlersContainer<EventHandlerType> & GetHandlersContainer();

	private:
		HandlersContainer<Handlers::MinionSummoned> minion_summoned_handlers_;
	};

	template <>
	HandlersContainer<Handlers::MinionSummoned> & Manager::GetHandlersContainer() {
		return this->minion_summoned_handlers_;
	}
}