#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

namespace EventManager
{
	namespace TriggerTypes
	{
		class BeforeMinionSummoned
		{
		public:
			typedef std::function<void(HandlersContainerController &, int)> FunctorType;
			typedef std::tuple<int> ArgsTuple;

			template <typename T,
				typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
			explicit BeforeMinionSummoned(T&& functor) : functor_(functor)
			{
			}

			void Handle(HandlersContainerController &controller, int v)
			{
				functor_(controller, v);
			}

		private:
			FunctorType functor_;
		};
	}
}