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
			class OnMinionPlay
			{
			public:
				typedef void (*FunctorType)(HandlersContainerController &, const Cards::Card &);
				typedef std::tuple<const Cards::Card &> ArgsTuple;

				template <typename T,
					typename std::enable_if_t<std::is_same<std::decay_t<T>, FunctorType>::value, nullptr_t> = nullptr>
					explicit OnMinionPlay(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, const Cards::Card & card)
				{
					functor_(controller, card);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}