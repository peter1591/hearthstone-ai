#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

#include "State/Types.h"
#include "State/Events/HandlersContainerController.h"

namespace state
{
	class State;
	namespace Cards
	{
		class Card;
	}

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
					state::CardRef card_ref_;
					state::Cards::Card const& card_;
					int amount_;
				};
			public:
				typedef std::function<void(HandlersContainerController &, Context&)> FunctorType;
				typedef std::tuple<const Cards::Card &, Context&> ArgsTuple;

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit OnHeal(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, Context & context)
				{
					functor_(controller, context);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}