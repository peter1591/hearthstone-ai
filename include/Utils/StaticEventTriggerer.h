#pragma once

#include <utility>

namespace Utils
{
	namespace StaticEventTriggererImpl
	{
		template <class... Triggers> class Invoker;

		template <> class Invoker<>
		{
		public:
			template <class... Args>
			static void Trigger(Args&&... args)
			{
			}
		};

		template <class Invokee>
		class Invoker<Invokee>
		{
		public:
			template <class... Args>
			static void Trigger(Args&&... args)
			{
				Invokee::Trigger(std::forward<Args>(args)...);
			}
		};

		template <class FirstType, class... RestTypes>
		class Invoker <FirstType, RestTypes...>
		{
		public:
			template <class... Args>
			static void Trigger(Args&&... args)
			{
				Invoker<FirstType>::Trigger(std::forward<Args>(args)...);
				Invoker<RestTypes...>::Trigger(std::forward<Args>(args)...);
			}
		};
	}

	template <class... Triggers>
	class StaticEventTriggerer
	{
	public:
		template <class... Args>
		static void Trigger(Args&&... args)
		{
			StaticEventTriggererImpl::Invoker<Triggers...>::Trigger(std::forward<Args>(args)...);
		}
	};
}