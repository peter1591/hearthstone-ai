#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

#include "FlowControl/Context/OnTakeDamage.h"
#include "State/Events/HandlersContainerController.h"

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
			class OnTakeDamage
			{
			public:
				typedef FlowControl::Context::OnTakeDamage & Parameter1;
				typedef std::function<void(HandlersContainerController &, Parameter1)> FunctorType;
				typedef std::tuple<const Cards::Card &, Parameter1> ArgsTuple;

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit OnTakeDamage(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, Parameter1 & parameter1)
				{
					functor_(controller, parameter1);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}