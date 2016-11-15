#pragma once

#include "FlowControl/ActionTypes.h"
#include "Manipulators/Manipulators.h"
#include "State/State.h"

namespace FlowControl
{
	template <class ActionParameterGetter, class RandomGenerator>
	class FlowController
	{
	public:
		FlowController(State::State & state, ActionParameterGetter & action_parameters, RandomGenerator & random)
			: state_(state), action_parameters_(action_parameters), random_(random)
		{

		}

		void PlayCard()
		{

		}

	public:
		State::State & state_;
		ActionParameterGetter & action_parameters_;
		RandomGenerator & random_;
	};
}