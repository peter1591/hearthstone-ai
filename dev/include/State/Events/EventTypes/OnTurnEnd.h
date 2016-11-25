#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>

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
				typedef std::function<void(HandlersContainerController &)> FunctorType;
				typedef std::tuple<const Cards::Card &> ArgsTuple;

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit OnTurnEnd(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller)
				{
					functor_(controller);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}