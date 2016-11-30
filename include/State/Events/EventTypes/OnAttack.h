#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>
#include "state/Types.h"
#include "FlowControl/Context/OnAttack.h"

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
			class OnAttack
			{
			public:
				typedef std::function<void(HandlersContainerController &, FlowControl::Context::OnAttack &)> FunctorType;
				typedef std::tuple<FlowControl::Context::OnAttack &> ArgsTuple;

				template <typename T> OnAttack(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, FlowControl::Context::OnAttack & context)
				{
					functor_(controller, context);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}