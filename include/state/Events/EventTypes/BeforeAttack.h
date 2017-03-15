#pragma once

#include <tuple>
#include <string>
#include <iostream>
#include <functional>
#include "state/Types.h"
#include "state/Cards/Card.h"
#include "FlowControl/FlowContext.h"

namespace state
{
	namespace Events
	{
		namespace EventTypes
		{
			class BeforeAttack
			{
			public:
				typedef void (*FunctorType)(HandlersContainerController &, CardRef, State &, CardRef);
				typedef std::tuple<CardRef, State &, CardRef> ArgsTuple;

				template <typename T> BeforeAttack(T&& functor) : functor_(functor) {}

				void Handle(HandlersContainerController &controller, CardRef attacker_, State & state_, CardRef defender_)
				{
					functor_(controller, attacker_, state_, defender_);
				}

			private:
				FunctorType functor_;
			};
		}
	}
}