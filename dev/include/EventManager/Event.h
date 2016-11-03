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

	template <typename EventHandlerType>
	class Event : public EventBase
	{
	public:
		using ArgsTuple = typename EventHandlerType::ArgsTuple;

		template <typename... Args>
		explicit Event(const Args&... args) : args_(args...) {}

		void TriggerEvent(Trigger& trigger)
		{
			constexpr size_t args_count = std::tuple_size<ArgsTuple>::value;
			return TriggerEventInternal(trigger, typename impl::gens<args_count>::type());
		}

	private:
		template <int... I>
		void TriggerEventInternal(Trigger& trigger, impl::seq<I...>) {
			return trigger.TriggerEvent<EventHandlerType>(std::get<I>(args_)...);
		}

	private:
		ArgsTuple args_;
	};

	template <typename EventHandlerType>
	class CategorizedEvent : public EventBase
	{
	public:
		typedef int CategoryType;
		using ArgsTuple = typename EventHandlerType::ArgsTuple;

		template <typename... Args>
		explicit CategorizedEvent(const CategoryType& category, const Args&... args) :
			category_(category), args_(args...)
		{
		}

		void TriggerEvent(Trigger& trigger)
		{
			constexpr size_t args_count = std::tuple_size<ArgsTuple>::value;
			return TriggerEventInternal(trigger, typename impl::gens<args_count>::type());
		}

	private:
		template <int... I>
		void TriggerEventInternal(Trigger& trigger, impl::seq<I...>) {
			return trigger.TriggerCategorizedEvent<CategoryType, EventHandlerType>(
				category_, std::get<I>(args_)...);
		}

	private:
		CategoryType category_;
		ArgsTuple args_;
	};
}