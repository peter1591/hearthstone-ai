#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

namespace state
{
	namespace Events
	{
		namespace EventTypes
		{
			class BeforeMinionSummoned
			{
			public:
				struct Context
				{
					state::State & state_;
					state::CardRef card_ref_;
					const state::Cards::Card & card_;
				};

			public:
				typedef std::function<void(HandlersContainerController &controller, Context&)> FunctorType;
				typedef std::tuple<Context&> ArgsTuple;

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit BeforeMinionSummoned(T&& functor) : functor_(functor)
				{
				}

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