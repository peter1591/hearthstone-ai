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
					state::FlowContext & flow_context_;
					state::CardRef defender_;
				};

			public:
				typedef std::function<void(HandlersContainerController &, CardRef, Context)> FunctorType;
				typedef std::tuple<CardRef, Context> ArgsTuple;

				template <typename T> OnAttack(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, CardRef attacker, Context context)
				{
					functor_(controller, attacker, std::move(context));
				}

			private:
				FunctorType functor_;
			};
		}
	}
}