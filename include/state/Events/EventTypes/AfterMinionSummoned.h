#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

namespace FlowControl { class FlowContext; }

namespace state
{
	namespace Events
	{
		namespace EventTypes
		{
			class AfterMinionSummoned
			{
			public:
				typedef void (*FunctorType)(HandlersContainerController &);
				typedef std::tuple<> ArgsTuple;

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit AfterMinionSummoned(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller)
				{
					functor_(controller);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}