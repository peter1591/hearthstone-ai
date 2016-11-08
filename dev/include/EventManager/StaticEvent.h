#pragma once

#include <utility>

namespace EventManager
{
	template <typename EventHandlerType>
	class StaticEvent : public EventBase
	{
	public:
		template <typename... Args>
		static void TriggerEvent(HandlersManager& mgr, Args&&... args)
		{
			return mgr.GetHandlersContainer<EventHandlerType>().TriggerAll(std::forward<Args>(args)...);
		}
	};

	template <typename EventHandlerType>
	class StaticCategorizedEvent : public EventBase
	{
	public:
		typedef int CategoryType;

		template <typename CategoryType_, typename... Args>
		static void TriggerEvent(HandlersManager& mgr, CategoryType_&& category, Args&&... args)
		{
			return mgr.GetHandlersContainer<CategoryType, EventHandlerType>()
				.TriggerAll(std::forward<CategoryType_>(category), std::forward<Args>(args)...);
		}
	};
}