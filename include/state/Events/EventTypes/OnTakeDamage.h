#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

#include "state/Events/HandlersContainerController.h"
#include "state/Cards/Card.h"
#include "FlowControl/FlowContext.h"

namespace state
{
	namespace Events
	{
		namespace EventTypes
		{
			class OnTakeDamage
			{
			public:
				struct Context
				{
					state::State & state_;
					FlowControl::FlowContext & flow_context_;
					state::Cards::Card const& card_;
					int damage_;
				};
			public:
				typedef void (*FunctorType)(HandlersContainerController &, state::CardRef, Context&);

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit OnTakeDamage(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, state::CardRef card_ref_, Context & context)
				{
					functor_(controller, card_ref_, context);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}