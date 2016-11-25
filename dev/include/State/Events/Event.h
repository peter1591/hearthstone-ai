#pragma once

#include <utility>
#include <tuple>
#include "State/Events/Manager.h"

namespace state
{
	namespace Events
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
			virtual void TriggerEvent(Manager&) = 0;
		};

		template <typename EventTriggerType>
		class Event : public EventBase
		{
		public:
			using ArgsTuple = typename EventTriggerType::ArgsTuple;

			template <typename U = EventTriggerType, typename... Args,
				typename std::enable_if_t<std::tuple_size<typename U::ArgsTuple>::value != 0, nullptr_t> = nullptr>
				explicit Event(Args&&... args) : args_(args...) {}

			template <typename U = EventTriggerType,
				typename std::enable_if_t<std::tuple_size<typename U::ArgsTuple>::value == 0, nullptr_t> = nullptr>
				Event() {}

			void TriggerEvent(Manager& mgr)
			{
				constexpr size_t args_count = std::tuple_size<ArgsTuple>::value;
				return TriggerEventInternal(mgr, typename impl::gens<args_count>::type());
			}

		private:
			template <int... I>
			void TriggerEventInternal(Manager& mgr, impl::seq<I...>) {
				return mgr.GetHandlersContainer<EventTriggerType>().TriggerAll(std::get<I>(args_)...);
			}

		private:
			ArgsTuple args_;
		};

		template <typename EventTriggerType>
		class CategorizedEvent : public EventBase
		{
		public:
			typedef int CategoryType;
			using ArgsTuple = typename EventTriggerType::ArgsTuple;

			template <typename U = EventTriggerType, typename... Args,
				typename std::enable_if_t<std::tuple_size<typename U::ArgsTuple>::value != 0, nullptr_t> = nullptr>
				explicit CategorizedEvent(const CategoryType& category, const Args&... args) :
				category_(category), args_(args...)
			{
			}

			template <typename U = EventTriggerType,
				typename std::enable_if_t<std::tuple_size<typename U::ArgsTuple>::value == 0, nullptr_t> = nullptr>
				explicit CategorizedEvent(const CategoryType& category) : category_(category)
			{
			}

			void TriggerEvent(Manager& mgr)
			{
				constexpr size_t args_count = std::tuple_size<ArgsTuple>::value;
				return TriggerEventInternal(mgr, typename impl::gens<args_count>::type());
			}

		private:
			template <int... I>
			void TriggerEventInternal(Manager& mgr, impl::seq<I...>) {
				return mgr.GetHandlersContainer<CategoryType, EventTriggerType>().TriggerAll(category_, std::get<I>(args_)...);
			}

		private:
			CategoryType category_;
			ArgsTuple args_;
		};
	}
}