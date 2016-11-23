#pragma once

#include <utility>

namespace StaticEventManager
{
	namespace impl
	{
		template <class FirstType, class... RestTypes>
		class Invoker2
		{
		public:
			template <class... Args>
			static void Trigger(Args&&... args)
			{
				FirstType::Trigger(std::forward<Args>(args)...);
				Invoker<RestTypes...>::Trigger(std::forward<Args>(args)...);
			}
		};

		template <class... Triggers> class Invoker
		{
		public:
			template <class... Args>
			static void Trigger(Args&&... args)
			{
				Invoker2<Triggers...>::Trigger(std::forward<Args>(args)...);
			}
		};

		template <> class Invoker<>
		{
		public:
			template <class... Args>
			static void Trigger(Args&&... args)
			{
			}
		};
	}

	template <class... Triggers>
	class Triggerer
	{
	public:
		template <class... Args>
		static void Trigger(Args&&... args)
		{
			impl::Invoker<Triggers...>::Trigger(std::forward<Args>(args)...);
		}
	};
}