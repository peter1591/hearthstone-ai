#pragma once

#include <utility>

namespace state
{
	namespace Events
	{
		template <typename EventTriggerType>
		class StaticEvent
		{
		public:
			template <typename... Args>
			static void TriggerEvent(HandlersManager& mgr, Args&&... args)
			{
				return mgr.GetHandlersContainer<EventTriggerType>().TriggerAll(std::forward<Args>(args)...);
			}
		};

		template <typename EventTriggerType>
		class StaticCategorizedEvent
		{
		public:
			typedef int CategoryType;

			template <typename CategoryType_, typename... Args>
			static void TriggerEvent(HandlersManager& mgr, CategoryType_&& category, Args&&... args)
			{
				return mgr.GetHandlersContainer<CategoryType, EventTriggerType>()
					.TriggerAll(std::forward<CategoryType_>(category), std::forward<Args>(args)...);
			}
		};
	}
}