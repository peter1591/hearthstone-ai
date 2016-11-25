#pragma once

#include <utility>

namespace state
{
	namespace Events
	{
		namespace impl
		{
			template <class FirstStaticEventType, class... RestStaticEventTypes>
			class StaticEventsInvoker2
			{
			public:
				template <class... Args>
				static void Trigger(Args&&... args)
				{
					FirstStaticEventType::TriggerEvent(std::forward<Args>(args)...);
					StaticEventsInvoker<RestStaticEventTypes...>::Trigger(std::forward<Args>(args)...);
				}
			};

			template <class... StaticEventTypes> class StaticEventsInvoker
			{
			public:
				template <class... Args>
				static void Trigger(Args&&... args)
				{
					StaticEventsInvoker2<StaticEventTypes...>::Trigger(std::forward<Args>(args)...);
				}
			};

			template <> class StaticEventsInvoker<>
			{
			public:
				template <class... Args>
				static void Trigger(Args&&... args)
				{
				}
			};
		}

		template <class... StaticEventTypes>
		class StaticEvents
		{
		public:
			template <class... Args>
			static void Trigger(Args&&... args)
			{
				impl::StaticEventsInvoker<StaticEventTypes...>::Trigger(std::forward<Args>(args)...);
			}
		};
	}
}