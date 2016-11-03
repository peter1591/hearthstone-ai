#pragma once

#include <utility>
#include <tuple>
#include "EventManager/Trigger.h"

namespace EventManager
{
	namespace impl
	{
		template<int ...>
		struct seq { };

		template<int N, int ...S>
		struct gens : gens<N - 1, N - 1, S...> { };

		template<int ...S>
		struct gens<0, S...> {
			typedef seq<S...> type;
		};
	}

	class EventBase
	{
	public:
		virtual ~EventBase() {}
		virtual void TriggerEvent(Trigger&) = 0;
	};

	template <typename EventHandlerType, typename... Args>
	class Event : public EventBase
	{
	public:
		explicit Event(const Args&... args) : args_(args...) {}

		void TriggerEvent(Trigger& trigger)
		{
			return TriggerEventInternal(trigger, typename impl::gens<sizeof...(Args)>::type());
		}

	private:
		template <int... I>
		void TriggerEventInternal(Trigger& trigger, impl::seq<I...>) {
			return trigger.TriggerEvent<EventHandlerType, Args...>(std::get<I>(args_)...);
		}

	private:
		std::tuple<Args...> args_;
	};

	template <typename CategoryType, typename EventHandlerType, typename... Args>
	class CategorizedEvent : public EventBase
	{
	public:
		explicit CategorizedEvent(const CategoryType& category, const Args&... args) :
			category_(category), args_(args...)
		{
		}

		void TriggerEvent(Trigger& trigger)
		{
			return TriggerEventInternal(trigger, typename impl::gens<sizeof...(Args)>::type());
		}

	private:
		template <int... I>
		void TriggerEventInternal(Trigger& trigger, impl::seq<I...>) {
			return trigger.TriggerCategorizedEvent<CategoryType, EventHandlerType, Args...>(
				category_, std::get<I>(args_)...);
		}

	private:
		CategoryType category_;
		std::tuple<Args...> args_;
	};
}