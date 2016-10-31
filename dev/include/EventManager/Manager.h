#pragma once

#include <utility>
#include "EventManager/HandlersContainer.h"
#include "EventManager/CategorizedHandlersContainer.h"
#include "EventManager/Handlers/MinionSummoned.h"
#include "EventManager/Handlers/MinionSummonedOnce.h"

namespace EventManager
{
	class Manager
	{
	public:
		template <typename EventHandlerType_>
		void PushBack(EventHandlerType_&& handler) {
			using EventHandlerType = typename std::remove_reference<EventHandlerType_>::type;
			GetHandlersContainer<EventHandlerType>().PushBack(std::forward<EventHandlerType>(handler));
		}

		template <typename Category, typename EventHandlerType_>
		void PushBack(Category&& category, EventHandlerType_&& handler) {
			using EventHandlerType = typename std::remove_reference<EventHandlerType_>::type;

			GetHandlersContainer<Category, EventHandlerType>().PushBack(
				std::forward<Category>(category), std::forward<EventHandlerType>(handler));
		}

		template <typename EventHandlerType_, typename... Args>
		void TriggerEvent(Args&&... args) {
			using EventHandlerType = typename std::remove_reference<EventHandlerType_>::type;

			GetHandlersContainer<EventHandlerType>().TriggerAll(std::forward<Args>(args)...);
		}

		template <typename Category, typename EventHandlerType_, typename... Args>
		void TriggerEvent(Category&& category, Args&&... args) {
			using EventHandlerType = typename std::remove_reference<EventHandlerType_>::type;

			GetHandlersContainer<Category, EventHandlerType>().TriggerAll(std::forward<Category>(category), std::forward<Args>(args)...);
		}

	private:
		template<typename EventHandlerType>
		HandlersContainer<EventHandlerType> & GetHandlersContainer();

		template<typename Category, typename EventHandlerType>
		CategorizedHandlersContainer<Category, EventHandlerType> & GetHandlersContainer();

	private:
		HandlersContainer<Handlers::MinionSummoned> minion_summoned_handlers_;
		HandlersContainer<Handlers::MinionSummonedOnce> minion_summoned_once_handlers_;

		CategorizedHandlersContainer<int, Handlers::MinionSummoned> categorized_minion_summoned_handlers_;
		CategorizedHandlersContainer<int, Handlers::MinionSummonedOnce> categorized_minion_summoned_once_handlers_;
	};

	template <>
	HandlersContainer<Handlers::MinionSummoned> & Manager::GetHandlersContainer() {
		return this->minion_summoned_handlers_;
	}

	template <>
	HandlersContainer<Handlers::MinionSummonedOnce> & Manager::GetHandlersContainer() {
		return this->minion_summoned_once_handlers_;
	}

	template <>
	CategorizedHandlersContainer<int, Handlers::MinionSummoned> & Manager::GetHandlersContainer() {
		return this->categorized_minion_summoned_handlers_;
	}

	template <>
	CategorizedHandlersContainer<int, Handlers::MinionSummonedOnce> & Manager::GetHandlersContainer() {
		return this->categorized_minion_summoned_once_handlers_;
	}
}