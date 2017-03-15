#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

#include "state/Types.h"
#include "state/Cards/Card.h"
#include "state/Events/HandlersContainerController.h"
#include "FlowControl/FlowContext.h"

namespace state
{
	namespace Events
	{
		namespace EventTypes
		{
			class OnHeal
			{
			public:
				struct Context
				{
					state::State & state_;
					state::Cards::Card const& card_;
					int amount_;
				};
			public:
				typedef void (*FunctorType)(HandlersContainerController &, state::CardRef, Context&);

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit OnHeal(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, state::CardRef card_ref, Context & context)
				{
					functor_(controller, card_ref, context);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}