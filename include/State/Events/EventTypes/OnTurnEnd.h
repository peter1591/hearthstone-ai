#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>
#include "FlowControl/Context/OnTurnEnd.h"

namespace state
{
	namespace Cards
	{
		class Card;
	}

	namespace Events
	{
		namespace EventTypes
		{
			class OnTurnEnd
			{
			public:
				typedef std::function<void(HandlersContainerController &, FlowControl::Context::OnTurnEnd &)> FunctorType;
				typedef std::tuple<FlowControl::Context::OnTurnEnd &> ArgsTuple;

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit OnTurnEnd(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, FlowControl::Context::OnTurnEnd & context)
				{
					functor_(controller, context);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}