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
			class BeforeAttack
			{
			public:
				typedef std::function<void(HandlersContainerController &, State &, CardRef const&, CardRef &)> FunctorType;
				typedef std::tuple<State &, CardRef const&, CardRef &> ArgsTuple;

				template <typename T> BeforeAttack(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, State & state_, CardRef & attacker_, CardRef & defender_)
				{
					functor_(controller, state_, attacker_, defender_);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}