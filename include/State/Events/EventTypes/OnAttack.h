#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>
#include "state/Types.h"

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
				struct Context
				{
					state::State & state_;
					FlowControl::FlowContext & flow_context_;
					state::CardRef attacker_;
					state::CardRef defender_;
				};

			public:
				typedef std::function<void(HandlersContainerController &, Context &)> FunctorType;
				typedef std::tuple<Context&> ArgsTuple;

				template <typename T> OnAttack(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, Context& context)
				{
					functor_(controller, context);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}