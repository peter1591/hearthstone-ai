#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

#include "FlowControl/Context/BeforeMinionSummoned.h"

namespace State
{
	namespace EventManager
	{
		namespace TriggerTypes
		{
			class BeforeMinionSummoned
			{
			public:
				typedef FlowControl::Context::BeforeMinionSummoned & Parameter1;
				typedef std::function<void(HandlersContainerController &controller, Parameter1)> FunctorType;
				typedef std::tuple<Parameter1> ArgsTuple;

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit BeforeMinionSummoned(T&& functor) : functor_(functor)
				{
				}

				void Handle(HandlersContainerController &controller, Parameter1 context)
				{
					functor_(controller, context);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}